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

	void eliminateFromHeap() {
		inHeap = false;
	}
};

struct radius {
	int index;
	double r;

	radius(int i, double R) {
		index = i;
		r = R;
	}
};

void swap(radius *a, radius *b) {
	radius temp = *a;
	*a = *b;
	*b = temp;
}

void insertElementToHeap(vector<radius> &heap, radius a) {
	heap.push_back(a);
	if(heap.size() > 1){
		int childIndex = heap.size();
		int parentIndex = int(childIndex / 2);
		
		while(heap[parentIndex - 1].r < a.r){
			//cout << heap[parentIndex - 1].r << " < " << heap[childIndex - 1].r <<endl;
			swap(heap[parentIndex - 1], heap[childIndex - 1]);
			if(parentIndex - 2 < 0) break;
			childIndex = parentIndex;
			parentIndex = int(childIndex / 2);
		}
	}
}

list<Triangle> listTriangles(vector<Vector2d> points, vector<radius> &heap) {
	list<Triangle> triangles;
	int n = points.size();
	
	for (int i = 0; i < n; i++) {
		Triangle t = Triangle(points[i % n], points[(i + 1) % n], points[(i + 2) % n]);
		triangles.push_back(t);
		insertElementToHeap(heap, radius((i + 1) % n, t.d));
	}

	return triangles;
}

vector<double> heapify(list<Triangle> triangles) {
	vector<double> heap;
	for (Triangle &triangle : triangles) {
		heap.push_back(triangle.d);
	}

	make_heap(heap.begin(), heap.end());

	return heap;
}

// Vector2d findPoint(double maxHeap, list<Triangle> triangles) {
// 	for (Triangle& triangle : triangles) {
// 		if (triangle.d == maxHeap) {
// 			return triangle.b;
// 			break;
// 		}
// 	}
// 	return Vector2d(0, 0);
// }

// vector<Vector2d> deletePoint(vector<Vector2d> points, Vector2d point) {
// 	for (int i = 0; i < points.size(); i++) {
// 		if ((points[i] - point).norm() == 0) {
// 			points.erase(points.begin() + i);
// 		}
// 	}
// 	return points;
// }

// vector<Vector2d> sch(vector<Vector2d> points) {
// 	double alpha = 10.5;

// 	list<Triangle> triangles = listTriangles(points);

// 	vector<double> heap = heapify(triangles);
// 	double maxHeap = heap.front();

// 	if (maxHeap > alpha) {
// 		Vector2d midpoint = findPoint(maxHeap, triangles);
// 		points = deletePoint(points, midpoint);
		
// 		return sch(points);
// 	}
// 	else {
		
// 		return points;
// 	}
// }

int main() {
	vector<Vector2d> points = { Vector2d(0.5, 3.25), Vector2d(3.14, 1.36), Vector2d(4.14, 4.84), Vector2d(0.14, 0.36), Vector2d(2.21, 1.45), Vector2d(2.21, 2.2)};

	int n = 1;
	for (auto i = points.begin(); i != points.end(); i++) {
		cout << "Point " << n << endl;
		cout << *i << endl;
		cout << "Length: " << (*i).norm() << endl;
		cout << '\n' << endl;
		n++;
	}

	vector<radius> radiusHeap;

	list<Triangle> triangles = listTriangles(points, radiusHeap);
	n = 1;
	for(auto i = triangles.begin(); i != triangles.end(); i++) {
		cout << "Triangle " << n << ": " << endl;
		cout << "Circum circle: " << (*i).d << '\n' << endl;
		n++;
	}

	cout << "\nHeap of cc radius" << endl;
	for(auto i = radiusHeap.begin(); i != radiusHeap.end(); i++)	cout << (*i).index << ' ' << (*i).r << endl;

	//vector<double> heap = heapify(triangles);

	// points = sch(points); 
	// cout << "SCH" << endl;
	// n = 1;
	// for (Vector2d& x : points) {
	// 	cout << "Point " << n << endl;
	// 	cout << x << '\n' << endl;
	// 	n++;
	// }

	return 0;
}