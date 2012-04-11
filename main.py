#/* This is a public domain A* pathfinder implement write by Zhicheng Wei @ Youpin */
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

from time import sleep

class node(object):
	def __init__(self, x, y): 
		self.x, self.y = (x, y)
		self.g = 0
		self.h = 0
		self.parent = None

	def __eq__(self, other): 
		return isinstance(other, node) and (self.x == other.x and self.y == other.y)

	def around(self):
		n1 = node(self.x - 1, self.y - 1)
		n2 = node(self.x    , self.y - 1)
		n3 = node(self.x + 1, self.y - 1)
		n4 = node(self.x - 1, self.y    )
		n5 = node(self.x + 1, self.y    )
		n6 = node(self.x - 1, self.y + 1)
		n7 = node(self.x    , self.y + 1)
		n8 = node(self.x + 1, self.y + 1)
		return (n1, n2, n3, n4, n5, n6, n7, n8)

	def getf(self):
		return self.g + self.h
	f = property(getf)


def h(position, goal):
	return abs(goal.x - position.x) + abs(goal.y - position.y)


def pathfinder(walls, start, goal):
	open = [start]
	close = []
	foot = []
	while open:
		n = min(open, key=lambda x: x.f)
		if n == goal:
			result = []
			while n.parent:
				result.append(n)
				n = n.parent
			return (result, foot)
		open.remove(n)
		close.append(n)
		for adjacent in n.around():
			if adjacent in walls:
				continue
			adjacent.g = n.g + 1
			if adjacent in open + close:
				continue
			adjacent.h = h(adjacent, goal)
			adjacent.parent = n
			open.append(adjacent)
			foot.append(adjacent)
	return []

width = 800
height = 400
box = 20
walls = []

start = None
end = None

foot = []
path = []

def drawBox(x, y):
	glBegin(GL_QUADS)
	glVertex3f(x * box, y * box, 0.0)
	glVertex3f(x * box + box, y * box, 0.0)
	glVertex3f(x * box + box, y * box + box, 0.0)
	glVertex3f(x * box, y * box + box, 0.0)
	glEnd()

def display():
	glClearColor(1.0, 1.0, 1.0, 1.0)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

	glColor3f(0.0, 0.0, 0.0)
	glLineWidth(1.0)
	glBegin(GL_LINES)
	for i in range(1, height / box + 1):
		glVertex3f(0.0, i * box, 0.0)
		glVertex3f(width, i * box, 0.0)
	for i in range(1, width / box + 1):
		glVertex3f(i * box, 0.0, 0.0)
		glVertex3f(i * box, height, 0.0)
	glEnd();

	for n in walls:
		drawBox(n.x, n.y)

	glColor3f(1.0, 0.0, 0.0)
	if start:
		drawBox(start.x, start.y)

	glColor3f(0.0, 0.0, 1.0)
	if end:
		drawBox(end.x, end.y)

	glColor3f(1.0, 1.0, 0.0)
	for n in foot:
		drawBox(n.x, n.y)

	glColor3f(0.0, 1.0, 0.0)
	for n in path:
		drawBox(n.x, n.y)

	glutSwapBuffers()

def reshape(w, h):
	glViewport(0, 0, w, h)
	glMatrixMode(GL_PROJECTION)
	glLoadIdentity()
	gluOrtho2D(0.0, w, 0.0, h)
	width, height = (w, h)
	glutPostRedisplay()

def keyboard(key, x, y):
	if ord(key) == 27:
		sys.exit()

def mouse(button, state, x, y):
	n = node(x/box, (height - y)/box)
	if button == GLUT_LEFT_BUTTON and state == GLUT_DOWN:
		if n in walls:
			walls.remove(n)
		else:
			walls.append(n)
	if button == GLUT_RIGHT_BUTTON and state == GLUT_DOWN:
		global start, end
		if start == n:
			start = end = None
		elif end == n:
			end = None
		elif start and not end:
			end = n
		elif end:
			end = n
		else:
			start = n
	if start and end:
		global foot, path
		(path, foot) = pathfinder(walls, start, end)
		foot = filter(lambda x: x not in path, foot)
		if len(path) == 0:
			print "can't find a way"
			
	glutPostRedisplay()


def init():
	glEnable(GL_DEPTH_TEST)

def main():
	glutInit()
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height)
	glutInitWindowPosition(200, 200)
	glutCreateWindow("main")
	init()
	glutDisplayFunc(display)
	glutReshapeFunc(reshape)
	glutKeyboardFunc(keyboard)
	glutMouseFunc(mouse)
	
	glutMainLoop();

if __name__ == '__main__':
	main()
