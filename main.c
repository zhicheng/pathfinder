/* This is a public domain A* pathfinder implement write by Zhicheng Wei @ Youpin */
#include <stdio.h>
#include <stdlib.h> 
#include <strings.h>
#include <math.h>
#include <time.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

typedef struct node_t 
{
	int x, y;
	int g;
	int f;
	struct node_t *parent;
	struct node_t *next;
} node_t;

typedef struct
{
	node_t *head;
} list_t;

node_t *
node_create(int x, int y)
{
	node_t *node = malloc(sizeof(node_t));
	bzero(node, sizeof(node_t));
	node->next = NULL;
	node->x = x;
	node->y = y;
	return node;
}

int
node_equal(node_t *a, node_t *b)
{
	return a->x == b->x && a->y == b->y;
}

node_t *
node_dup(node_t *node)
{
	return node_create(node->x, node->y);
}

list_t *
list_create()
{
	list_t *list = malloc(sizeof(list_t));	
	bzero(list, sizeof(list_t));	
	list->head = node_create(0, 0);
	return list;
}

void
list_destroy(list_t *list)
{
	node_t *prev = list->head;
	node_t *next;
	while (prev != NULL) {
		next = prev->next;
		free(prev);
		prev = next;
	}
	free(list);
}


int
list_empty(list_t *list)
{
	return list->head->next == NULL;
}

int
list_has(list_t *list, node_t *node)
{
	node_t *prev = list->head;
	while ((prev = prev->next) != NULL)
		if ((node->x == prev->x) && (node->y == prev->y))
			return 1;
	return 0;
}

void
list_append(list_t *list, node_t *node)
{
	
	node_t *prev = list->head;
	while (prev->next != NULL) {
		prev = prev->next;
	}
	node->next = NULL;
	prev->next = node;
}

node_t *
list_lowest(list_t *list)
{
	node_t *node = list->head;
	node_t *lowest_node = NULL;
	unsigned int lowest_value = -1;
	while ((node = node->next) != NULL) {
		if (node->f < lowest_value) {
			lowest_value = node->f;
			lowest_node = node;
		}
	}
	return lowest_node;
}

node_t *
list_remove(list_t *list, node_t *node)
{
	node_t *prev = list->head;
	while (prev->next != NULL) {
		if (node->x == prev->next->x && 
		    node->y == prev->next->y) {
			node = prev->next;
			prev->next = prev->next->next;
			return node;
		}
		prev = prev->next;
	}
	return NULL;
}

node_t *
around(node_t *node, int direction)
{
	/* direction 0~9. 4 is original point */
	return node_create(node->x + direction % 3 - 1, node->y + direction / 3 - 1);
}

void
list_print(list_t *list)
{
	node_t *node = list->head;
	while ((node = node->next))
		fprintf(stderr, "(%d, %d),", node->x, node->y);
	fprintf(stderr, "\n");
}


int 
h(node_t *node, node_t *end)
{
	return abs(node->x - end->x) + abs(node->y - end->y);
}

list_t *
pathfinder(list_t *walls, node_t *start, node_t *goal)
{
	int i;
	list_t *openlist = list_create();
	list_t *closelist = list_create();

	list_append(openlist, node_dup(start));
	
	while (!list_empty(openlist)) {
		node_t *n = list_lowest(openlist);
		if (node_equal(n, goal)) {
			list_t *list = list_create();
			list_append(list, node_dup(n));
			while ((n = n->parent) != NULL)
				list_append(list, node_dup(n));
			list_destroy(openlist);
			list_destroy(closelist);
			return list;
		}
		list_append(closelist, list_remove(openlist, n));
		for (i = 0; i < 9; i++) {
			node_t *adjacent = around(n, i);
			if (list_has(walls, adjacent) ||
			    list_has(openlist, adjacent) ||
			    list_has(closelist, adjacent)) {
				free(adjacent);
				continue;
			}
			adjacent->g = n->g + 1;
			adjacent->parent = n;
			adjacent->f = h(adjacent, goal) + adjacent->g + (i % 2 == 0);
			list_append(openlist, adjacent);
		}
	}
	list_destroy(openlist);
	list_destroy(closelist);
	return NULL;
}

static list_t *walls;
static list_t *ways;

int width = 800;
int height = 800;
int box = 20;

static node_t *start, *end;
void init() 
{
	walls = list_create();
	start = node_create(0, 0);
	end = node_create(0, 0);
	glEnable(GL_DEPTH_TEST);
}

void keyboard(unsigned char key, int x, int y) 
{
	switch (key) {
	case 27: exit(0);
	}
}

void mouse(int button, int state, int x, int y)
{
	int col = (int)floor(x/box);
	int row = (int)floor((height - y)/box);
	node_t *node = node_create(col, row);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (!list_has(walls, node)) {
			list_append(walls, node);
		} else {
			free(list_remove(walls, node));
			free(node);
		}
	} else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		if (node_equal(start, node)) {
			start->y = end->y = start->x = end->x = 0;
		} else if (node_equal(end, node)) {
			end->y = end->x = 0;
		} else if (start->y > 0) {
			end->y = row;
			end->x = col;
		} else {
			start->y = row;
			start->x = col;
		}
		free(node);
	}
	if (start->x > 0 && end->x > 0) {
		clock_t t;
		if (ways)
			list_destroy(ways);
		t = clock();
		ways = pathfinder(walls, start, end);
		t = clock() - t;
		printf("pathfinder: %f\n", (double)t/CLOCKS_PER_SEC);
	}
		
	glutPostRedisplay();
}

void resize(int w, int h) 
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	gluOrtho2D(0.0, w, 0.0, h);
	width = w;
	height = h;
	glutPostRedisplay();
}

void
DrawBox(int x, int y, int size)
{
	glBegin(GL_QUADS);
		glVertex3f(x * size, y * size, 0.0f);
		glVertex3f(x * size + size, y * size, 0.0f);
		glVertex3f(x * size + size, y * size + size, 0.0f);
		glVertex3f(x * size, y * size + size, 0.0f);
	glEnd();
}

void display() 
{
	int i;
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glColor3f(0.0f, 0.0f, 0.0f);
	glLineWidth(1.0);
	glBegin(GL_LINES);
	for (i = 1; i <= height/box; i++) {
		glVertex3f(0.0f, i * box, 0.0f);
		glVertex3f(width, i * box, 0.0f);
	}
	for (i = 1; i <= width/box; i++) {
		glVertex3f(i * box, 0.0f, 0.0f);
		glVertex3f(i * box, height, 0.0f);
	}
	glEnd();

	if (walls) {
		node_t *node = walls->head;
		while ((node = node->next) != NULL)
			DrawBox(node->x, node->y, box);
	}


	glColor3f(1.0f, 0.0f, 0.0f);
	if (start->x > 0)
		DrawBox(start->x, start->y, box);

	glColor3f(0.0f, 0.0f, 1.0f);
	if (end->x > 0)
		DrawBox(end->x, end->y, box);

	if (ways) {
		node_t *node = ways->head;
		while ((node = node->next) != NULL)
			DrawBox(node->x, node->y, box);
	}
	
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height); 
	glutInitWindowPosition(200, 200);
	
	glutCreateWindow("main");
	init();
	
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	
	glutMainLoop();
	return 0; 
}

