#include <iostream>
#include <random>
#include <chrono>

constexpr size_t Number_Of_Values = 10000000;

constexpr size_t RunCount = 10;

// struktura uzlu binarniho stromu
struct TTree_Node {
	double value;
	TTree_Node* left;
	TTree_Node* right;

	TTree_Node(double v) : value(v), left(nullptr), right(nullptr) {}
};

// vkladani prvku do stromu, zatrizeni klasicky jen podle velikosti
void Insert_Recursively(TTree_Node* root, double value) {
	if (value < root->value) {
		if (root->left == nullptr) {
			root->left = new TTree_Node(value);
		}
		else {
			Insert_Recursively(root->left, value);
		}
	}
	else {
		if (root->right == nullptr) {
			root->right = new TTree_Node(value);
		}
		else {
			Insert_Recursively(root->right, value);
		}
	}
}

// funkce pro uvolneni pameti - rekurzivne projde strom a smaze vsechny uzly
void Destroy_Tree(TTree_Node* node) {
	if (node == nullptr) {
		return;
	}

	Destroy_Tree(node->left);
	Destroy_Tree(node->right);
	delete node;
}

// opet funkce pro mereni casu (spusti dry run, a pak nekolikrat znovu, zmeri cas a zprumeruje)
void Do_Measure(const std::string& name, TTree_Node* root, size_t(*fnc)(TTree_Node*))
{
	std::cout << "Measurement: " << name << std::endl;

	std::cout << "Dry run ...";
	fnc(root);
	std::cout << " OK" << std::endl;

	unsigned long long tm = 0;

	for (size_t i = 0; i < RunCount; i++)
	{
		std::cout << "Run " << i << " ... ";

		auto st = std::chrono::steady_clock::now();

		size_t cnt = fnc(root);

		std::cout << "Counted " << cnt << " leaves, ";

		auto end = std::chrono::steady_clock::now();
		tm += std::chrono::duration_cast<std::chrono::milliseconds>(end - st).count();

		std::cout << "OK" << std::endl;
	}
	tm /= RunCount;

	std::cout << "Average time: " << tm << "ms" << std::endl << std::endl;
}

// [SERIOVA VERZE] funkce pro spocitani listu stromu - rekurzivne projde strom a spocita pocet listu
size_t Count_Leaves_Serial(TTree_Node* node) {
	if (node == nullptr) {
		return 0;
	}

	if (node->left == nullptr && node->right == nullptr) {
		return 1;
	}

	return Count_Leaves_Serial(node->left) + Count_Leaves_Serial(node->right);
}

// [OPENMP VERZE] funkce pro spocitani listu stromu
size_t Count_Leaves_OpenMP(TTree_Node* node) {
	if (node == nullptr) {
		return 0;
	}
	if (node->left == nullptr && node->right == nullptr) {
		return 1;
	}
	size_t left_leaves = 0;
	size_t right_leaves = 0;

	// parallel sections - spusti sekce uvnitr sebe paralelne (kazdou ve svem vlakne), jedna pro levou vetev a druha pro pravou vetev
#pragma omp parallel sections
	{
#pragma omp section // jedno vlakno se vyda tudy
		{
			left_leaves = Count_Leaves_OpenMP(node->left);
		}
#pragma omp section // druhe vlakno se vyda tudy
		{
			right_leaves = Count_Leaves_OpenMP(node->right);
		}
	}

	return left_leaves + right_leaves;
}

// Pro ty z vas, kteri jsou na LLVM a vyssi verzi OpenMP - ta by mela byt podstatne efektivnejsi
/*
size_t Count_Leaves_Task(TTree_Node* node) {

	if (node->left == nullptr && node->right == nullptr) {
		return 1;
	}

	size_t left = 0, right = 0;

#pragma omp task shared(left)
	left = node->left ? Count_Leaves_Task(node->left) : 0;

#pragma omp task shared(right)
	right = node->right ? Count_Leaves_Task(node->right) : 0;

#pragma omp taskwait
	return left + right;
}

size_t Count_Leaves_OpenMP_Task(TTree_Node* root) {
	size_t result = 0;

#pragma omp parallel
	{
#pragma omp single
		{
			result = Count_Leaves_Task(root);
		}
	}

	return result;
}
*/

int main(int argc, char** argv) {

	// generator s fixnim seedem, abychom meli stejna data pro vsechna mereni
	std::mt19937 gen(42);

	// rovnomerne rozdeleni hodnot mezi 0 a 100
	std::uniform_real_distribution<> dis(0.0, 100.0);

	std::cout << "Generating data ... " << std::endl;

	// vlozime korenovy uzel
	TTree_Node* root = new TTree_Node(0);

	for (size_t i = 0; i < Number_Of_Values; i++) {
		const double value = dis(gen);
		Insert_Recursively(root, value);
	}

	Do_Measure("Serial", root, &Count_Leaves_Serial);
	Do_Measure("OpenMP", root, &Count_Leaves_OpenMP);

	// viz vyse
	//Do_Measure("OpenMP with tasks", root, &Count_Leaves_OpenMP_Task);

	Destroy_Tree(root);
	return 0;
}
