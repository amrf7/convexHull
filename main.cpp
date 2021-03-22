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
		if((*i).inHull){
			cout << "Point " << n << endl;
			cout << (*i).point << endl;
			cout << "Length: " << (*i).point.norm() << endl;
			cout << "inHull: " << (*i).inHull << endl;
			cout << '\n' << endl;
		}
		
		n++;
	}
}

void printTriangles(const list<Triangle> &triangles) {
	int n = 0;
	for(auto i = triangles.begin(); i != triangles.end(); i++) {
		if((*i).inHeap) {
			cout << "Triangle " << n << ": " << endl;
			cout << "a\t\tb\t\tc" << endl;
			cout << (*i).a.norm() << "\t\t" << (*i).b.norm() << "\t\t" << (*i).c.norm() << endl;
			cout << "Circum circle: " << (*i).d << endl;
			cout << "inHeap: " << (*i).inHeap << '\n' << endl;
		}
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

//void compareToChild

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

void compareToChild(vector<Radius> &heap) {
	int parentIndex = 1;
	int childIndex = 2 * parentIndex;
	while(childIndex < heap.size() && heap[parentIndex - 1].radius < heap[childIndex - 1].radius || heap[parentIndex - 1].radius < heap[childIndex].radius){
		if(heap[childIndex - 1].radius < heap[childIndex].radius) childIndex++;
		while(heap[parentIndex - 1].radius < heap[childIndex - 1].radius){
			swap(heap[parentIndex - 1], heap[childIndex - 1]);
			parentIndex = childIndex;
			childIndex = 2 * parentIndex;
		}
	}
}

void removeHeap(vector<Radius> &heap){
	//cout << "\nREMOVE HEAP. HEAP SIZE: " << heap.size() << endl;
	//cout << "swap(" << heap.front().radius << ", " << heap.back().radius << ");" << endl;
	swap(heap.front(), heap.back());
	//cout << "\nHEAP SWAPPED " << endl;
	//printHeap(heap);
	heap.pop_back();
	//cout << "\nHEAP popped" << endl;
	//printHeap(heap);
	int parentIndex = 1;
	int childIndex = 2 * parentIndex;
	while(childIndex + 1 < heap.size() && (heap[parentIndex - 1].radius < heap[childIndex - 1].radius || heap[parentIndex - 1].radius < heap[childIndex].radius)){
		if(heap[childIndex - 1].radius < heap[childIndex].radius) childIndex++;
		if(heap[parentIndex - 1].radius < heap[childIndex - 1].radius){
			swap(heap[parentIndex - 1], heap[childIndex - 1]);
			parentIndex = childIndex;
			childIndex = 2 * parentIndex;
			//cout << "\nHEAP reorganized. child index: " << childIndex << "<"  << endl;
			//printHeap(heap);

		}
	}
}

bool checkIfHeapExists(vector<Radius> &heap, list<Triangle> &triangles){
	list<Triangle>::iterator it = triangles.begin();
	advance(it, heap[0].triangleIndex);
	cout << "Triangle " << heap[0].triangleIndex << endl;
	cout << "Circumcircle radius: " << (*it).d;
	cout << "\tIn Heap: " << (*it).inHeap << endl;
	return (*it).inHeap;
}

void updateTriangles(const vector<Point> &points, list<Triangle> &triangles, vector<Radius> &heap ){
	int n = points.size();
	int m = triangles.size();
	int t1Index = n + heap[0].midpointIndex - 1;
	int t2Index = n + heap[0].midpointIndex + 1;
	// cout << "HEAP BEFORE REMOVAL" << endl;
	// printHeap(heap);
	// cout << "NEW HEAP" << endl;
	removeHeap(heap);
	//printHeap(heap);
	cout << "\nt1: " << t1Index % n <<" t2: " << t2Index % n << '\n' << endl;
	Triangle t = Triangle(points[(t1Index - 1) % n].point, points[(t1Index) % n].point, points[(t1Index + 2) % n].point);
	Radius r = Radius(t1Index % n, m, t.d);
	triangles.push_back(t);
	insertElementToHeap(heap, r);
	Triangle t1 = Triangle(points[(t2Index - 2) % n].point, points[t2Index % n].point, points[(t2Index + 1) % n].point);
	Radius r1 = Radius(t2Index % n, m + 1, t1.d);
	triangles.push_back(t1);
	insertElementToHeap(heap, r1);
}

void sch(vector<Point> &points){
	double alpha = 5.2;

	vector<Radius> radiusHeap;
	list<Triangle> triangles = listTriangles(points, radiusHeap);
	
	// printPoints(points);
	// printTriangles(triangles);
	// printHeap(radiusHeap);

	while(radiusHeap[0].radius > alpha && checkIfHeapExists(radiusHeap, triangles)) {
		cout << "\nRemove Point " << radiusHeap[0].midpointIndex << endl;
		removePointFromHull(points, triangles, radiusHeap[0]);
		updateTriangles(points, triangles, radiusHeap);
		printPoints(points);
		printTriangles(triangles);
		printHeap(radiusHeap);

		while(!checkIfHeapExists(radiusHeap, triangles)) {
			removeHeap(radiusHeap);
			cout << "NEW HEAP" << endl;
			printHeap(radiusHeap);
		}
	}
}

int main() {
	vector<Point> points = { Point(Vector2d(0.5, 3.25)), Point(Vector2d(3.14, 1.36)), Point(Vector2d(4.14, 4.84)), 
							Point(Vector2d(0.14, 0.36)), Point(Vector2d(2.21, 1.45)), Point(Vector2d(2.21, 2.2))};

	sch(points);
	cout << "\nPoints in convex Hull" << endl;
	printPoints(points);

	return 0;
}