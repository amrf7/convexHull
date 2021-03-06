#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <memory>
#include <math.h> 
#include <Eigen/Dense>
#include <fstream>

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
		d = findCircumcircleRadius();
	}

	double findCircumcircleRadius() {
		double A = (a - b).norm();
		double B = (b - c).norm();
		double C = (a - c).norm();
		double s = (A + B + C) / 2;
		return (A*B*C) / (4*sqrt(s*(s-A)*(s-B)*(s-C)));
	}

	void removeFromHeap() {
		inHeap = false;
	}
};

struct Radius {
	int frontpointIndex, midpointIndex, endpointIndex, triangleIndex;
	double radius;

	Radius(int fpIndex, int mpIndex, int epIndex, int tIndex, double R) {
		frontpointIndex = fpIndex;
		midpointIndex = mpIndex;
		endpointIndex = epIndex;
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

bool checkHull(const vector<Vector2d> &points, vector<Point> originalPoints, double r);

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

/* Heap insertion while maintaining heap */
void insertElementToHeap(vector<Radius> &heap, Radius a) {
	heap.push_back(a);
	if(heap.size() > 1){
		int childIndex = heap.size();
		int parentIndex = int(childIndex / 2);
		
		while(heap[parentIndex - 1].radius < a.radius){
			swap(heap[parentIndex - 1], heap[childIndex - 1]);
			if(parentIndex - 2 < 0) break;
			childIndex = parentIndex;
			parentIndex = int(childIndex / 2);
		}
	}
}

/* Listing triangles and inserting their circumcircle radius to the heap */
list<Triangle> listTriangles(vector<Point> points, vector<Radius> &heap) {
	list<Triangle> triangles;
	int n = points.size();
	
	for (int i = 0; i < n; i++) {
		Triangle t = Triangle(points[i % n].point, points[(i + 1) % n].point, points[(i + 2) % n].point);
		Radius r = Radius(i % n, (i + 1) % n, (i + 2) % n, i, t.d);
		triangles.push_back(t);
		insertElementToHeap(heap, r);
	}

	return triangles;
}

/* Changes the inHull Boleean in the respective point */
void removePointFromHull(vector<Point> &points, list<Triangle> &triangles, const Radius &heap) {
	list<Triangle>::iterator it;
	int trianglesSize = triangles.size();
	int trianglesIndex =  trianglesSize + heap.triangleIndex;
	for(int i = -1; i <= 1; i++){
		it = triangles.begin();
		if((trianglesIndex + i) >= 2 * points.size() && heap.triangleIndex < points.size()) {
			advance(it, (points.size() + heap.triangleIndex + i) % points.size());
		} else{
			advance(it, (trianglesIndex + i) % trianglesSize);
		}	

		(*it).removeFromHeap();
	}
	points[heap.midpointIndex].removeFromHull();
}

/* Heap operation, comparing node to left and right child */
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

/* Remove max heap while mantaining heap */
void removeHeap(vector<Radius> &heap){
	swap(heap.front(), heap.back());
	heap.pop_back();
	int parentIndex = 1;
	int childIndex = 2 * parentIndex;
	while(childIndex + 1 < heap.size() && (heap[parentIndex - 1].radius < heap[childIndex - 1].radius || heap[parentIndex - 1].radius < heap[childIndex].radius)){
		if(heap[childIndex - 1].radius < heap[childIndex].radius) childIndex++;
		if(heap[parentIndex - 1].radius < heap[childIndex - 1].radius){
			swap(heap[parentIndex - 1], heap[childIndex - 1]);
			parentIndex = childIndex;
			childIndex = 2 * parentIndex;
		}
	}
}

/* Verifies all points corresponding to the max heap exist in the sch */
bool checkIfMaxHeapIsInHull(vector<Radius> &heap, vector<Point> points, list<Triangle> &triangles){
	list<Triangle>::iterator it = triangles.begin();
	advance(it, heap[0].triangleIndex);
	bool trianglePointsExist = points[heap[0].frontpointIndex].inHull && (points[heap[0].midpointIndex].inHull && points[heap[0].endpointIndex].inHull);
	return (*it).inHeap && trianglePointsExist;
}

/* Finds the closest previous point in the Hull*/
int findPreviousPoint(const vector<Point> &points, int pointIndex) {
	while(!points[pointIndex % points.size()].inHull) {
		pointIndex--;
	}
	return pointIndex;
}

/* Finds the closest next point in the Hull */
int findNextPoint(const vector<Point> &points, int pointIndex) {
	while(!points[pointIndex % points.size()].inHull) {
		pointIndex++;
	}
	return pointIndex;
}

/* Makes two new triangles based on the point that must be deleted from the sch */
void makeTriangles(const vector<Point> &points, list<Triangle> &triangles, vector<Radius> &heap, int previousMidpoint) {
	int n = points.size();

	int newMidpoint = findPreviousPoint(points, n + previousMidpoint - 1);

	int newFrontpoint = findPreviousPoint(points, newMidpoint - 1);

	int newEndpoint = findNextPoint(points, n + previousMidpoint + 1); 

	Triangle newTriangle1 = Triangle(points[newFrontpoint % n].point, points[newMidpoint % n].point, points[newEndpoint % n].point);
	Radius newRadius1 = Radius(newFrontpoint % n, newMidpoint % n, newEndpoint % n, triangles.size(), newTriangle1.d);

	newFrontpoint = newMidpoint;
	newMidpoint = newEndpoint;
	newEndpoint = findNextPoint(points, newMidpoint + 1);

	Triangle newTriangle2 = Triangle(points[newFrontpoint % n].point, points[newMidpoint % n].point, points[newEndpoint % n].point);
	Radius newRadius2 = Radius(newFrontpoint % n, newMidpoint % n, newEndpoint % n, triangles.size() + 1, newTriangle2.d);

	triangles.push_back(newTriangle1);
	triangles.push_back(newTriangle2);
	insertElementToHeap(heap, newRadius1);
	insertElementToHeap(heap, newRadius2);
}

/* Stores the index of the max heap midpoint, removes it from the heap and gets the two new triangles*/
void updateTriangles(const vector<Point> &points, list<Triangle> &triangles, vector<Radius> &heap ){
	int eliminatedPointIndex = heap.front().midpointIndex;
	removeHeap(heap);
	makeTriangles(points, triangles, heap, eliminatedPointIndex);
}

/* Strictly Convex Hull */
vector<Vector2d> sch(vector<Point> &points, double alpha){
	if(points.size() < 3) {
		cout << "You need at least 3 points.\n" << endl;
		return {};
	} 
	vector<Vector2d> strictlyConvexHull;

	vector<Radius> radiusHeap;
	list<Triangle> triangles = listTriangles(points, radiusHeap);

	while(radiusHeap[0].radius > alpha && checkIfMaxHeapIsInHull(radiusHeap, points, triangles)) {
		removePointFromHull(points, triangles, radiusHeap[0]);
		updateTriangles(points, triangles, radiusHeap);

		while(!checkIfMaxHeapIsInHull(radiusHeap, points, triangles)) {
			removeHeap(radiusHeap);
		}
	}
	for (auto i = points.begin(); i != points.end(); i++) {
		if((*i).inHull){
			strictlyConvexHull.push_back((*i).point);
		}
	}

	if(checkHull(strictlyConvexHull, points, alpha)) cout << "Strictly convex" << endl;
	else cout << "Not strictly convex" << endl;
	
	return strictlyConvexHull;

}

bool checkHull(const vector<Vector2d> &points, vector<Point> originalPoints, double r) {
    int n = points.size();
    for(int i = 0; i < n; i++) {
        double x1 = points[i % n][0], y1 = points[i % n][1];
        double x2 = points[(i + 1) % n][0], y2 = points[(i + 1) % n][1];
        double xa = (x2 - x1)/2, ya = (y2 - y1)/2;
        Vector2d rhombusCenter = Vector2d(x1 + xa, y1 + ya);
        double a = sqrt(pow(xa,2) + pow(ya,2));
        double b = sqrt(pow(r,2) - pow(a,2));

        Vector2d distanceToCenter = Vector2d(b * ya / a, -b * xa / a);
        Vector2d center1 = rhombusCenter + distanceToCenter;
        Vector2d center2 = rhombusCenter - distanceToCenter;

        double m = originalPoints.size();
        for(int j = 0; j < m; j++) {
            double distanceToCenter1 = (originalPoints[j].point - center1).norm();
            double distanceToCenter2 = (originalPoints[j].point - center2).norm();

            if(distanceToCenter1 > r + 1e10) return false;
        }
    }

	return true;
}

int main() {
	string pointsInfo;
	ifstream readFile;
	double x, y;
	vector <Point> points;

	readFile.open("points.txt");
	if(!readFile) {
		cout << "File not found." << endl;
		exit(1);
	}

	while(getline(readFile, pointsInfo)) {
		// get substring and convert to double
		x = atof(pointsInfo.substr(0, pointsInfo.find(';')).c_str());
		y = atof(pointsInfo.substr(pointsInfo.find(';') + 1, pointsInfo.length()).c_str());
		// add point to vector
		points.push_back(Point(Vector2d(x, y)));
	}

	vector<Vector2d> schPoints = sch(points, 2.5);

	cout << "\nPoints in strictly convex hull" << endl;
	for(auto i = schPoints.begin(); i != schPoints.end(); i++) cout << *i << '\n' << endl;

	return 0;
}