#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <memory>
#include <math.h> 
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

struct Triangle {
	Vector2d a, b, c;
	double d; //circumcircle diameter
	bool inHeap = true;

	Triangle(Vector2d A, Vector2d B, Vector2d C) {
		a = A;
		b = B;
		c = C;
		d = findCircumcircleDiameter();
	}

	double findCircumcircleDiameter() {
		double ab = (a - b).norm();
		double bc = (b - c).norm();
		double ac = (a - c).norm();
		return (ab*bc*ac) / sqrt((ab+bc+ac)*(bc+ac-ab)*(ac+ab-bc)*(ab+bc-ac));
	}

	void removeFromHeap() {
		inHeap = false;
	}
};

struct Radius {
	int midpointIndex, triangleIndex;
	double radius;

	Radius(int pIndex, int tIndex, double R) {
		midpointIndex = pIndex;
		triangleIndex = tIndex;
		radius = R;
	}
};

struct Point {
	Vector2d point;
	bool inHull = true;

	Point(Vector2d p) {
		point = p;
	}

	void removeFromHull() {
		inHull = false;
	}
};

void printPoints(const vector<Point> &points) {
	int n = 0;
	for (auto i = points.begin(); i != points.end(); i++) {
		cout << "Point " << n << endl;
		cout << (*i).point << endl;
		cout << "Length: " << (*i).point.norm() << endl;
		cout << "inHull: " << (*i).inHull << endl;
		cout << '\n' << endl;
		n++;
	}
}

void printTriangles(const list<Triangle> &triangles) {
	int n = 0;
	for(auto i = triangles.begin(); i != triangles.end(); i++) {
		cout << "Triangle " << n << ": " << endl;
		cout << "a\t\tb\t\tc" << endl;
		cout << (*i).a.norm() << "\t\t" << (*i).b.norm() << "\t\t" << (*i).c.norm() << endl;
		cout << "Circum circle: " << (*i).d << endl;
		cout << "inHeap: " << (*i).inHeap << '\n' << endl;
		n++;
	}
}

void printHeap(const vector<Radius> heap) {
	cout << "\nHeap of cc Radius" << endl;
	for(auto i = heap.begin(); i != heap.end(); i++){
		cout << "midpointIndex\ttriangleIndex\tcircumCircle Radius" << endl;
		cout << (*i).midpointIndex << "\t\t" << (*i).triangleIndex << "\t\t" << (*i).radius << endl;
	}
}


void swap(Radius *a, Radius *b) {
	Radius temp = *a;
	*a = *b;
	*b = temp;
}

void insertElementToHeap(vector<Radius> &heap, Radius a) {
	heap.push_back(a);
	if(heap.size() > 1){
		int childIndex = heap.size();
		int parentIndex = int(childIndex / 2);
		
		while(heap[parentIndex - 1].radius < a.radius){
			//cout << heap[parentIndex - 1].r << " < " << heap[childIndex - 1].r <<endl;
			swap(heap[parentIndex - 1], heap[childIndex - 1]);
			if(parentIndex - 2 < 0) break;
			childIndex = parentIndex;
			parentIndex = int(childIndex / 2);
		}
	}
}

list<Triangle> listTriangles(vector<Point> points, vector<Radius> &heap) {
	list<Triangle> triangles;
	int n = points.size();
	
	for (int i = 0; i < n; i++) {
		Triangle t = Triangle(points[i % n].point, points[(i + 1) % n].point, points[(i + 2) % n].point);
		Radius r = Radius((i + 1) % n, i, t.d);
		triangles.push_back(t);
		insertElementToHeap(heap, r);
	}

	return triangles;
}

void removePointFromHull(vector<Point> &points, list<Triangle> &triangles, const Radius &heap) {
	list<Triangle>::iterator it;
	int trianglesSize = triangles.size();
	int trianglesIndex =  trianglesSize + heap.triangleIndex;
	for(int i = -1; i <= 1; i++){
		it = triangles.begin();
		advance(it, (trianglesIndex + i) % trianglesSize);
		(*it).removeFromHeap();
	}
	points[heap.midpointIndex].removeFromHull();
}

void updateTriangles(vector<Point> points,  list<Triangle> triangles){
	
}

void sch(vector<Point> &points){
	double alpha = 11.251;

	vector<Radius> radiusHeap;
	list<Triangle> triangles = listTriangles(points, radiusHeap);
	
	printPoints(points);
	printTriangles(triangles);
	printHeap(radiusHeap);

	if(radiusHeap[0].radius > alpha) {
		cout << "\nTriangles size" << triangles.size() << endl;
		removePointFromHull(points, triangles, radiusHeap[0]);
		printPoints(points);
		printTriangles(triangles);
	}
}

int main() {
	vector<Point> points = { Point(Vector2d(0.5, 3.25)), Point(Vector2d(3.14, 1.36)), Point(Vector2d(4.14, 4.84)), 
							Point(Vector2d(0.14, 0.36)), Point(Vector2d(2.21, 1.45)), Point(Vector2d(2.21, 2.2))};

	sch(points);

	return 0;
}