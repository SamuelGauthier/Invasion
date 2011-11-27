#include "md2loader.h"

md2_animations anims[20] = 
{
	// first, last, fps
    {   0,  39,  9 },   // STAND
    {  40,  45, 10 },   // RUN
    {  46,  53, 10 },   // ATTACK
    {  54,  57,  7 },   // PAIN_A
    {  58,  61,  7 },   // PAIN_B
    {  62,  65,  7 },   // PAIN_C
    {  66,  71,  7 },   // JUMP
    {  72,  83,  7 },   // FLIP
    {  84,  94,  7 },   // SALUTE
    {  95, 111, 10 },   // FALLBACK
    { 112, 122,  7 },   // WAVE
    { 123, 134,  6 },   // POINT
    { 135, 153, 10 },   // CROUCH_STAND
    { 154, 159,  7 },   // CROUCH_WALK
    { 160, 168, 10 },   // CROUCH_ATTACK
    { 169, 172,  7 },   // CROUCH_PAIN
    { 173, 177,  5 },   // CROUCH_DEATH
    { 178, 183,  7 },   // DEATH_FALLBACK
    { 184, 189,  7 },   // DEATH_FALLFORWARD
    { 190, 197,  7 },   // DEATH_FALLBACKSLOW
};

GMD2Model* loadMD2(const char* filename)
{
	GMD2Model* m = new GMD2Model;
	FILE* input = fopen(filename, "rb");
	md2_header header;

	// Voir si le chargement s'est fait avec succès
	assert(input);

	// Lire l'en-tête
	fread((void*)&header, sizeof(md2_header), 1, input);

	// Voir si c'est un fichier md2
	assert((header.ident == MD2_IDENT) && (header.version == MD2_VERSION));

	// Charger les triangles
	m->num_tris = header.num_tris;
	m->indices = new md2_tris[header.num_tris];
	fseek(input, header.ofs_tris, SEEK_SET);
	fread((void*)m->indices, sizeof(md2_tris), header.num_tris, input);

	// Charger les coordonnées de textures
	m->num_texcoords = header.num_st;
	m->texcoords = new md2_st[header.num_st];
	fseek(input, header.ofs_st, SEEK_SET);
	fread((void*)m->texcoords, sizeof(md2_st), header.num_st, input);

	m->skinwidth = (float)header.skin_width;
	m->skinheight = (float)header.skin_height;

	// Charger les frames
	m->num_frames = header.num_frames;
	m->frames = new md2_frames[header.num_frames];

	fseek(input, header.ofs_frames, SEEK_SET);
	for(int i=0;i<header.num_frames;i++)
	{
		// Allouer l'espace
		m->frames[i].first = new md2_xyz[header.num_xyz];

		fread((void*)&m->frames[i].scale, sizeof(float) * 3, 1, input);
		fread((void*)&m->frames[i].translate, sizeof(float) * 3, 1, input);
		fread((void*)&m->frames[i].name, sizeof(char) * 16, 1, input);
		fread((void*)m->frames[i].first, sizeof(md2_xyz), header.num_xyz, input);
	}

	setAnimation(m, 0);

	fclose(input);
	addRelease(releaseMD2Model, (void*)m);
	return m;
}

void setTexture(GMD2Model* m, const char* filename)
{
	m->tex = loadTexture(filename);

	glBindTexture(GL_TEXTURE_2D, m->tex);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void render(GMD2Model* m, int frame)
{
	frame = frame % m->num_frames;
	// texture
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, m->tex);

	// Recupère les données pour dessiner
	md2_frames* cur = &m->frames[frame];
	md2_xyz* first = cur->first;

	// Dessin
	glBegin(GL_TRIANGLES);

	for(int i = 0;i < m->num_tris;i++)
	{
		for(int j = 0;j < 3;j++)
		{
			md2_xyz* vert = &first[m->indices[i].vertexindex[j]];
			md2_st* texcoord = &m->texcoords[m->indices[i].textureindex[j]];

			float u = (float)texcoord->s/m->skinwidth;
			float v = (float)texcoord->t/m->skinheight;

			glTexCoord2f(u, v);
			glVertex3f((float)vert->v[0] * cur->scale[0] + cur->translate[0],
					   (float)vert->v[1] * cur->scale[1] + cur->translate[1],
					   (float)vert->v[2] * cur->scale[2] + cur->translate[2]);
		}
	}

	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void setAnimation(GMD2Model* m, int animation_id)
{
	m->time = 0.f;
	m->anim = &anims[animation_id];
	m->frame = m->anim->first_frame;
}

void render(GMD2Model* m, float ElapsedTime)
{
	// séléctionne la frame
	float interval = 1000.f/(float)m->anim->fps;
	
	if(m->time >= interval)
	{
		m->frame = m->next_frame;

		m->time -= interval;
	}

	m->next_frame = m->frame+1;
	if(m->frame == m->anim->last_frame)
	{
		m->next_frame = m->anim->first_frame;
	}

	render(m, m->frame, m->next_frame, m->time/interval);

	m->time+=ElapsedTime;
}

/*
void render(GMD2Model* m, int framea, int frameb, float inter)
{

	// texture
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, m->tex);

	// Recupère les données pour dessiner
	md2_frames* cur1 = &m->frames[framea];
	md2_frames* cur2 = &m->frames[frameb];
	md2_xyz* first1 = cur1->first;
	md2_xyz* first2 = cur2->first;

	// Dessin
	glBegin(GL_TRIANGLES);

	for(int i = 0;i < m->num_tris;i++)
	{
		for(int j = 0;j < 3;j++)
		{
			md2_st* texcoord = &m->texcoords[m->indices[i].textureindex[j]];
			float u = (float)texcoord->s/m->skinwidth;
			float v = (float)texcoord->t/m->skinheight;
			glTexCoord2f(u, v);

			md2_xyz* vert1 = &first1[m->indices[i].vertexindex[j]];
			md2_xyz* vert2 = &first2[m->indices[i].vertexindex[j]];
			

			float x1 = (float)vert1->v[0] * cur1->scale[0] + cur1->translate[0];
			float y1 = (float)vert1->v[1] * cur1->scale[1] + cur1->translate[1];
			float z1 = (float)vert1->v[2] * cur1->scale[2] + cur1->translate[2];
			float x2 = (float)vert2->v[0] * cur2->scale[0] + cur2->translate[0];
			float y2 = (float)vert2->v[1] * cur2->scale[1] + cur2->translate[1];
			float z2 = (float)vert2->v[2] * cur2->scale[2] + cur2->translate[2];

			glVertex3f(x1 + (x2 - x1) * inter,
					   y1 + (y2 - y1) * inter,
					   z1 + (z2 - z1) * inter);
		}
	}

	glEnd();
	glDisable(GL_TEXTURE_2D);
}
*/

void render(GMD2Model* m, int framea, int frameb, float inter)
{

	// texture
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, m->tex);

	// Recupère les données pour dessiner
	md2_frames* cur1 = &m->frames[framea];
	md2_frames* cur2 = &m->frames[frameb];
	md2_xyz* first1 = cur1->first;
	md2_xyz* first2 = cur2->first;

	// Dessin
	glBegin(GL_TRIANGLES);

	for(int i = 0;i < m->num_tris;i++)
	{
		for(int j = 0;j < 3;j++)
		{
			md2_st* texcoord = &m->texcoords[m->indices[i].textureindex[j]];
			float u = (float)texcoord->s/m->skinwidth;
			float v = (float)texcoord->t/m->skinheight;
			glTexCoord2f(u, v);

			md2_xyz* vert1 = &first1[m->indices[i].vertexindex[j]];
			md2_xyz* vert2 = &first2[m->indices[i].vertexindex[j]];
			

			float x1 = (float)vert1->v[0] * cur1->scale[0] + cur1->translate[0];
			float y1 = (float)vert1->v[1] * cur1->scale[1] + cur1->translate[1];
			float z1 = (float)vert1->v[2] * cur1->scale[2] + cur1->translate[2];
			float x2 = (float)vert2->v[0] * cur2->scale[0] + cur2->translate[0];
			float y2 = (float)vert2->v[1] * cur2->scale[1] + cur2->translate[1];
			float z2 = (float)vert2->v[2] * cur2->scale[2] + cur2->translate[2];

			glVertex3f(x1 + (x2 - x1) * inter,
					   y1 + (y2 - y1) * inter,
					   z1 + (z2 - z1) * inter);
		}
	}

	glEnd();
	glDisable(GL_TEXTURE_2D);
}

char* getAnimationName(GMD2Model* m)
{
	return m->frames[m->anim->first_frame].name;
}

void releaseMD2Model(void* a)
{
	GMD2Model* m = (GMD2Model*)a;
	
	glDeleteTextures(1, &m->tex);

	for(int i=0;i<m->num_frames;i++)
		delete[] m->frames[i].first;
	delete[] m->frames;
	delete[] m->indices;

	delete m;
}