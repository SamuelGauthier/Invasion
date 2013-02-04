#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string.h>
using namespace std;


struct Vec3f
{
	float x,y,z;
	Vec3f(float _x=0.f, float _y=0.f, float _z=0.f)
	{
		x = _x; y = _y; z = _z;
	}

}; /* Vec3f */

struct Vec2f
{
	float x,y;
	Vec2f(float _x=0.f, float _y=0.f)
	{
		x = _x; y = _y;
	}

	static Vec2f inverse(Vec2f& v)
	{
		return Vec2f(v.x, -v.y);
	}
}; /* Vec2f */

struct Tri
{
	Vec3f v0;
	Vec3f n0;
	Vec2f t0;

	Vec3f v1;
	Vec3f n1;
	Vec2f t1;

	Vec3f v2;
	Vec3f n2;
	Vec2f t2;
};

int strcharcnt(const char* line, char model)
{
	int result = 0;
	while(*line)
	{
		if(*line == model)
			result++;
		line++;
	}
	return result;
}

inline void writeVec3f(const Vec3f* v, FILE* f)
{
	fwrite((void*)&v->x, 4, 1, f);
	fwrite((void*)&v->y, 4, 1, f);
	fwrite((void*)&v->z, 4, 1, f);
}

inline void writeVec2f(const Vec2f* v, FILE* f)
{
	fwrite((void*)&v->x, 4, 1, f);
	fwrite((void*)&v->y, 4, 1, f);
}


bool readInputFile(vector<Tri>& tris, FILE* f)
{
	vector<Vec3f> vertices, normals;
	vector<Vec2f> texcoords;

	char line[64];

	while(fgets(line, 64, f))
	{
		if(line[0] == 'v' && line[1] == ' ')
		{
			static float x,y,z;
			sscanf(line, "v %f %f %f",&x,&y,&z);
			vertices.push_back(Vec3f(x,y,z));
		}

		else if(line[0] == 'v' && line[1] == 'n')
		{
			static float x,y,z;
			sscanf(line, "vn %f %f %f",&x,&y,&z);
			normals.push_back(Vec3f(x,y,z));
		}

		else if(line[0] == 'v' && line[1] == 't')
		{
			static float u,v;
			sscanf(line, "vt %f %f",&u,&v);
			texcoords.push_back(Vec2f(u,v));
		}

		else if(line[0] == 'f')
		{
			static int v1, v2, v3, v4;
			static int n1, n2, n3, n4;
			static int t1, t2, t3, t4;
			static Tri triangle[2];
			if(!normals.size()){
				fprintf(stderr, "ERROR : no normals found\n");
				return false;
			}

			if(!texcoords.size()){
				fprintf(stderr, "ERROR : no texcoords found\n");
				return false;
			}

			// Quad
			if(strcharcnt(line, '/') == 8){
				sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3, &v4, &t4, &n4);

				triangle[0].v0 = vertices[v1 - 1];
				triangle[0].v1 = vertices[v2 - 1];
				triangle[0].v2 = vertices[v4 - 1];
				triangle[1].v0 = vertices[v2 - 1];
				triangle[1].v1 = vertices[v3 - 1];
				triangle[1].v2 = vertices[v4 - 1];

				triangle[0].n0 = normals[n1 - 1];
				triangle[0].n1 = normals[n2 - 1];
				triangle[0].n2 = normals[n4 - 1];
				triangle[1].n0 = normals[n2 - 1];
				triangle[1].n1 = normals[n3 - 1];
				triangle[1].n2 = normals[n4 - 1];

				triangle[0].t0 = Vec2f::inverse(texcoords[t1 - 1]);
				triangle[0].t1 = Vec2f::inverse(texcoords[t2 - 1]);
				triangle[0].t2 = Vec2f::inverse(texcoords[t4 - 1]);
				triangle[1].t0 = Vec2f::inverse(texcoords[t2 - 1]);
				triangle[1].t1 = Vec2f::inverse(texcoords[t3 - 1]);
				triangle[1].t2 = Vec2f::inverse(texcoords[t4 - 1]);


				tris.push_back(triangle[0]);
				tris.push_back(triangle[1]);
			}

			// Triangle
			else
			{
				sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
				triangle[0].v0 = vertices[v1 - 1];
				triangle[0].v1 = vertices[v2 - 1];
				triangle[0].v2 = vertices[v3 - 1];

				triangle[0].n0 = normals[n1 - 1];
				triangle[0].n1 = normals[n2 - 1];
				triangle[0].n2 = normals[n3 - 1];

				triangle[0].t0 = Vec2f::inverse(texcoords[t1 - 1]);
				triangle[0].t1 = Vec2f::inverse(texcoords[t2 - 1]);
				triangle[0].t2 = Vec2f::inverse(texcoords[t3 - 1]);

				tris.push_back(triangle[0]);
			}
		}

	}

	return true;
}

bool isAnimFile(const char* name)
{
	// the anim files are like name_000001.obj
	return (strstr(name, "00000") != 0);
}

void incAnimFile(char* filename)
{
	char* cursor = strchr(filename, '.') - 1;
	if(cursor)
	{
		*cursor = *cursor + 1;
		while(*cursor == ':')
		{
			*cursor = '0';
			cursor--;
			*cursor = *cursor + 1;
		}
	}
}

char* justFileName(char* path)
{
	char* prev = path;
	char* cur = path;
	while(cur)
	{
		prev = cur;
		cur = strchr(prev+1, '\\');
		if(!cur) cur = strchr(prev+1, '/');

		if(cur) cur++;
	}

	return prev;
}

char* searchchr(char* str, char c)
{
    while(*str)
    {
        if(*str == c)
            return str;
        str++;
    }
    return NULL;
}
void writeOutputFile(vector<Tri>& tris, FILE* f)
{
	Tri* ts = new Tri[tris.size()];
	for(unsigned int i=0;i<tris.size();i++)
	{
		ts[i].v0 = tris[i].v0;
		ts[i].v1 = tris[i].v1;
		ts[i].v2 = tris[i].v2;

		ts[i].n0 = tris[i].n0;
		ts[i].n1 = tris[i].n1;
		ts[i].n2 = tris[i].n2;

		ts[i].t0 = tris[i].t0;
		ts[i].t1 = tris[i].t1;
		ts[i].t2 = tris[i].t2;
	}
	fwrite((void*)&ts[0], sizeof(Tri), tris.size(), f);
	delete[] ts;
}

int main(int argc, const char *argv[])
{
	if(argc < 2){
		fprintf(stderr, "USAGE : converter.exe [OBJ FILE]\n");
		return EXIT_FAILURE;
	}

	// ----------- Name Parsing and Modification ----------
	char finput_name[128];
	char foutput_name[128];
	FILE *foutput = 0, *finput = 0;

	bool first_time = true, success = false;
	int numTri = 0;
	vector<Tri> tris;

	// Test if it's an animation
	strncpy(finput_name, argv[1], 128);
	bool animation = isAnimFile(finput_name);
	// write an output name
	strncpy(foutput_name, argv[1], 128);
	char* point_pos = searchchr(foutput_name, '.');
	*point_pos = '\0';
	// add the extension
	strcat(foutput_name, ".imf");
	// open the output file
	foutput = fopen(foutput_name, "wb");
	// Loop once if it's not an animation, otherwise loop until the last frame
	while(true)
	{
		// open the obj file
		finput = fopen(finput_name, "r");
		if(!finput){
			if(first_time){
				fprintf(stderr, "ERROR : the file could not be found %s\n", finput_name);
				break;
			}
			else{
				success = true;
				break;
			}
		}

		if(!readInputFile(tris, finput))
			break;

		// print the input file
		printf("%s ", justFileName(finput_name));

		// if it's for the first time ( espacially for animated obj ) write num Triangles
		if(first_time)
		{
			numTri = tris.size();
			fwrite((void*)&numTri, 4, 1, foutput);

			first_time = false;
		}

		// writing
		writeOutputFile(tris, foutput);
		tris.clear();

		// close the input file
		fclose(finput);

		// if it's an animation, increment the number in the file name
		if(animation) incAnimFile(finput_name);
		else{
            success = true;
            break;
		}
	}

	fclose(foutput);

	// print infos
	if(success){
		printf("-> %s\n\n", justFileName(foutput_name));
		printf("Triangles : %d\n", numTri);
	}

	else {
		printf("EXIT_FAILURE\n");
		system("PAUSE");
	}

	return 0;
}
