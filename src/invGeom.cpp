#include "invMath.h"
#include "invTex.h"
#include "invBase.h"

#include "invGeom.h"
#define LIGHTMAP_SIZE 10

//------------------------------------------------
// Terrain
//------------------------------------------------
int Terrain::width = 0, Terrain::length = 0;
float Terrain::size = 1.f;
float** Terrain::data = NULL;
GLuint Terrain::tex = 0;

void Terrain::init()
{
	Terrain::width = 0; Terrain::length = 0;
	Terrain::data = NULL;

	GarbageCollector::add(Terrain::destroy, NULL);
}

void Terrain::destroy(void*)
{
	if(Terrain::data)
	{
		for(int i=0;i<Terrain::width;i++)
		{
			delete[] Terrain::data[i];
		}

		delete[] Terrain::data;
		Terrain::data = NULL;
	}
}

GLuint Terrain::generateLightmap()
{
	Uint8 data[LIGHTMAP_SIZE * LIGHTMAP_SIZE * 3];

	GLuint tex;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &tex);

	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	
	return tex;

}

void Terrain::render()
{
	glColor3f(1.f, 1.f, 1.f);
	float x = (-(float)(Terrain::width-1)/2.f)*size;
	for(int i=0;i<width-1;i++)
	{
		float z = -((float)(Terrain::length-1)/2.f)*size;
		glBegin(GL_TRIANGLE_STRIP);
		
		for(int j=0;j<length;j++)
		{
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, (float)i/0.2f, (float)j/0.2f);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB, (float)i/(float)(width-1), (float)j/(float)(length-1));
			glVertex3f(x, data[i][j], z);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, (float)(i+1)/0.2f, (float)j/0.2f);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB, (float)(i+1)/(float)(width-1), (float)j/(float)(length-1));
			glVertex3f(x + size, data[i+1][j], z);

			z+=size;
		}
		glEnd();

		x+=size;
	}
}

//------------------------------------------------
// IMF
//------------------------------------------------
IMF::Model* IMF::load(const char* filename)
{
	Model* m = new Model;
	FILE* f = fopen(filename, "rb");
	assert(f);
	fread((void*)&m->numTri, 4, 1, f);
	m->t = new Triangle[m->numTri];
	fread((void*)&m->t[0], sizeof(Triangle), m->numTri, f);
	fclose(f);
	GarbageCollector::add(deinit, (void*)m);
	return m;
}

void IMF::render(void* tmp, float)
{
	Model* m = (Model*)tmp;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m->tex);
	glBegin(GL_TRIANGLES);
	for(int i=0;i<m->numTri;i++)
	{
		for(int j=0;j<3;j++)
		{
			glTexCoord2f(m->t[i].t[j].x, 1.f - m->t[i].t[j].y);
			glNormal3f(m->t[i].n[j].x, m->t[i].n[j].y, m->t[i].n[j].z);
			glVertex3f(m->t[i].v[j].x, m->t[i].v[j].y, m->t[i].v[j].z);
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void IMF::deinit(void* tmp)
{
	Model* m = (Model*)tmp;

	delete[] m->t;
	delete m;
}
//------------------------------------------------
// MD2
//------------------------------------------------
MD2::Animation anims[20] = 
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

MD2::Model* MD2::load(const char* filename)
{
	Model* m = new Model;
	FILE* input = fopen(filename, "rb");
	Header header;
	assert(input);
	fread((void*)&header, sizeof(Header), 1, input);
	assert((header.ident == MD2_IDENT) && (header.version == MD2_VERSION));

	m->num_tris = header.num_tris;
	m->num_st = header.num_st;
	m->num_frames = header.num_frames;
	m->skinwidth = header.skin_width;
	m->skinheight = header.skin_height;

	// Triangles
	m->tris = new Tri[header.num_tris];
	fseek(input, header.ofs_tris, SEEK_SET);
	fread((void*)m->tris, sizeof(Tri), header.num_tris, input);
	
	// Texcoords
	m->texcoords = new St[header.num_st];
	fseek(input, header.ofs_st, SEEK_SET);
	fread((void*)m->texcoords, sizeof(St), header.num_st, input);
	
	// Frames
	m->frames = new Frame[header.num_frames];

	fseek(input, header.ofs_frames, SEEK_SET);
	for(int i=0;i<header.num_frames;i++)
	{
		// Allouer l'espace
		m->frames[i].first = new XYZ[header.num_xyz];

		fread((void*)&m->frames[i].scale, sizeof(float) * 3, 1, input);
		fread((void*)&m->frames[i].translate, sizeof(float) * 3, 1, input);
		fread((void*)&m->frames[i].name, sizeof(char) * 16, 1, input);
		fread((void*)m->frames[i].first, sizeof(XYZ), header.num_xyz, input);
	}
	setAnimation(m, 0);

	fclose(input);
	GarbageCollector::add(deinit, (void*)m);
	return m;
}

void MD2::render(void* tmp, float ElapsedTime)
{
	// séléctionne la frame
	Model* m = (Model*)tmp;
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

	if(Game::texture)
		m->time+=ElapsedTime;
}

void MD2::render(MD2::Model* m, int framea, int frameb, float inter)
{
	// texture
	if(Game::texture){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m->tex);
	}

	// Recupère les données pour dessiner
	Frame* cur1 = &m->frames[framea];
	Frame* cur2 = &m->frames[frameb];
	XYZ* first1 = cur1->first;
	XYZ* first2 = cur2->first;

	// Dessin
	glBegin(GL_TRIANGLES);

	for(int i = 0;i < m->num_tris;i++)
	{
		for(int j = 0;j < 3;j++)
		{
			if(Game::texture){
				St* texcoord = &m->texcoords[m->tris[i].textureindex[j]];
				float u = (float)texcoord->s/m->skinwidth;
				float v = (float)texcoord->t/m->skinheight;
				glTexCoord2f(u, v);
			}

			XYZ* vert1 = &first1[m->tris[i].vertexindex[j]];
			XYZ* vert2 = &first2[m->tris[i].vertexindex[j]];
			

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
	if(Game::texture)
		glDisable(GL_TEXTURE_2D);
}

void MD2::setAnimation(void* tmp, int animation_id)
{
	Model* m = (Model*)tmp;
	m->time = 0.f;
	m->anim = &anims[animation_id];
	m->frame = m->anim->first_frame;
}

void MD2::deinit(void* tmp)
{
	Model* m = (Model*)tmp;

	glDeleteTextures(1, &m->tex);
	for(int i=0;i<m->num_frames;i++)
		delete[] m->frames[i].first;
	delete[] m->frames;
	delete[] m->tris;
	delete[] m->texcoords;

	delete m;
}
