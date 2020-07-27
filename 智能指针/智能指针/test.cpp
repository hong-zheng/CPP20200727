#define _CRT_SECURE_NO_WARNINGS 1
#include <memory>
#include <iostream>
#include <vector>
#include <mutex>
#include <atomic> // ԭ�Ӳ���
#include <exception>
#include <thread>
using namespace std;
	


template<class T,class deletor>
class sharedPtr{
public:
	sharedPtr(T* ptr)
		:_ptr(ptr)
		, _useCount(new int)
		, _mutex(new mutex)
	{}

	~sharedPtr()
	{
		//���������ü����Լ�
		if (subRef() == 0){
			delete _ptr;
			delete _useCount;
			delete _mutex;
			cout << "~sharedPtr" << endl;
		}
		//�ж����ü���
	}

	sharedPtr(const sharedPtr<T>& sp)
		:_ptr(sp._sp)
		, _useCount(sp._useCount)
		, _mutex(sp._mutex)
	{
		addRef();
	}

	sharedPtr<T>& operator=(const sharedPtr<T> sp){
		//if (this != &sp) //ֻ�й������Դ��ͬ�ű�ʾ��ͬ��
		if (_ptr!=sp._ptr)
		{
			//�жϵ�ǰ�������Դ�Ƿ���Ҫ�ͷ�
			if (--(*_useCount) == 0){
				delete _ptr;
				delete _useCount;
				delete _mutex;
			}
			//��ֵ
			_ptr = sp._ptr;
			_useCount = sp._ptr;
			_mutex = sp._mutex;
			addRef();
		}
		return *this;
	}

	int getCount(){
		return *_useCount;
	}

	int addRef(){
		_mutex.lock();
		++(*_useCount);
		_mutex.unlock();
		return *_useCount;
	}

	int subRef()
	{
		_mutex.lock();
		--(*_useCount);
		_mutex.unlock();
		return *_useCount;
	}
private:
	T* _ptr;
	// ��Ҫ��_useCount�Ĳ�����װ��һ�����в���
	int* _useCount;
	mutex* _mutex;
	deletor* _del;
};


class A{
public:
	A(){}

	~A()
	{
		cout << "~A()" << endl;
	}
private:
	int _a;
};

template<class T>
struct deleteArray{
	void operator()(T* ptr)
	{
		delete[] ptr;
		cout << "delete Array" << endl;
	}
};
//void test(){
//	shared_ptr<A> sp(new A[100]);
//}

void test1(){
	deleteArray<A> da;
	// ���þ������������������Ŀռ�����޸�
	shared_ptr<A> sp(new A[100],da);
}

template<class T>
struct freeM{
	void operator()(T* ptr)
	{
		free(ptr);
		cout << "free" << endl;
	}
};
// shared_ptr�ڶ�����������ɾ��
void test(){
	freeM<int> fm;
	shared_ptr<int> sp(new int, fm);
}
int main(){
	test();


	system("pause");
	return 0;
}

// �����������������
// �ݹ麯�����ݹ���
// ��������RAII˼�룬��ֹδ�ͷ�


// ����ָ�룺 
// 1��ʵ��RAII˼��
// 2��ʵ��ָ��Ĺ���

//template<class T>
//class smartPtr{
//public:
//	smartPtr(T* ptr)
//		:_ptr(ptr)
//	{
//		cout << "smartPtr(T* ptr)" << endl;
//	}
//
//	// ����Ȩת��
//	smartPtr(smartPtr<T>& sp)
//		:_ptr(sp._ptr)
//	{
//		sp._ptr = nullptr;
//	}
//
//	~smartPtr()
//	{
//		if (_ptr)
//		{
//			cout << "~smartPtr()" << endl;
//			delete _ptr;
//		}
//	
//	}
//
//	smartPtr<T>& operator=(smartPtr<T>& sp)
//	{
//		if (this != &sp){
//			// �ͷ�ԭ����Դ
//			if (_ptr)
//			{
//				delete _ptr;
//			}
//			// ����Ȩת��
//			_ptr = sp._ptr;
//			sp._ptr = nullptr;
//		}
//		return *this;
//	}
//	T& operator*()
//	{
//		return *_ptr;
//	}
//
//	T* operator->()
//	{
//		return _ptr;
//	}
//private:
//	T* _ptr;
//};
//
//class Date{
//public:
//	int _year = 1;
//};
//void test1(){
//	int* ptr = new int;
//	smartPtr<int> sp(ptr); //������������ʼ��
//	// ��������ָ��������ɶ����ͷŵ�����
//	//smartPtr<int> sp2(ptr);
//
//	//������ʼ��
//	smartPtr<int> sp3(new int);
//	
//}
//void test2(){
//	smartPtr<Date> sp4(new Date);
//	(*sp4)._year = 100;
//	cout << sp4->_year << endl;
//	sp4->_year = 2020;
//	cout << (*sp4)._year << endl;
//}
//void test3(){
//	// ������ʼ��
//	//smartPtr<Date> sp(new Date);
//	//// �����ͷ�
//	//smartPtr<Date> copy = sp;
//
//	Date* ptr = new Date;
//	Date* ptr2 = ptr;
//
//	delete ptr;
//}
//
//
//// ����Ȩת��
//void test5(){
//	// һ���ǲ��ã��н������쳣�ķ��� 
//	auto_ptr<Date> ap(new Date);
//	ap->_year = 2020;
//	cout << "ap->_year = " << ap->_year << endl;
//	// ����ʱ��������Ȩת��
//	// ����ת�ƺ󣬱�ת�Ƶ�ָ�벻����ʹ��
//	auto_ptr<Date> copy = ap;
//	// ��������Ȩת��֮������ָ�벻��ӵ����Դ���ʲ��ܷ�����Դ
//	cout << "copy->_year = "<< copy->_year << endl;
//	//cout << "ap->_year = " << ap->_year << endl;
//}
//
//void test6(){
//	unique_ptr<Date> up(new Date);
//	// unique_ptr :������-->��������͸�ֵ���������Ϊdelete����
//	//unique_ptr<Date> copy = up;
//	unique_ptr<Date> up2(new Date);
//
//	//up2 = up;
//}
//
//template<class T>
//class  uniquePtr{
//public:
//	uniquePtr(T* ptr)
//		:_ptr(ptr)
//	{}
//
//	~uniquePtr()
//	{
//		if (_ptr)
//			delete _ptr;
//	}
//
//	T* operator->()
//	{
//		return _ptr;
//	}
//
//	T& operator*()
//	{
//		return *_ptr;
//	}
//
//	// ������
//	uniquePtr(const uniquePtr<T>& up) = delete;
//	uniquePtr<T>& operator = (const uniquePtr<T>& up) = delete;
//
//private:
//	T* _ptr;
//};
//
//// �������ü�����ǳ����
//// ��¼��ǰ��Դ������ָ��������Ϊ����
//
////shared_ptr<class T> : �̰߳�ȫ
//template<class T>
//class sharedPtr{
//public:
//	sharedPtr(T* ptr)
//		:_ptr(ptr)
//		, _useCount(new int)
//		, _mutex(new mutex)
//	{}
//
//	~sharedPtr()
//	{
//		//���������ü����Լ�
//		if (subRef() == 0){
//			delete _ptr;
//			delete _useCount;
//			delete _mutex;
//			cout << "~sharedPtr" << endl;
//		}
//		//�ж����ü���
//	}
//
//	sharedPtr(const sharedPtr<T>& sp)
//		:_ptr(sp._sp)
//		, _useCount(sp._useCount)
//		, _mutex(sp._mutex)
//	{
//		addRef();
//	}
//
//	sharedPtr<T>& operator=(const sharedPtr<T> sp){
//		//if (this != &sp) //ֻ�й������Դ��ͬ�ű�ʾ��ͬ��
//		if (_ptr!=sp._ptr)
//		{
//			//�жϵ�ǰ�������Դ�Ƿ���Ҫ�ͷ�
//			if (--(*_useCount) == 0){
//				delete _ptr;
//				delete _useCount;
//				delete _mutex;
//			}
//			//��ֵ
//			_ptr = sp._ptr;
//			_useCount = sp._ptr;
//			_mutex = sp._mutex;
//			addRef();
//		}
//		return *this;
//	}
//
//	int getCount(){
//		return *_useCount;
//	}
//
//	int addRef(){
//		_mutex.lock();
//		++(*_useCount);
//		_mutex.unlock();
//		return *_useCount;
//	}
//
//	int subRef()
//	{
//		_mutex.lock();
//		--(*_useCount);
//		_mutex.unlock();
//		return *_useCount;
//	}
//private:
//	T* _ptr;
//	// ��Ҫ��_useCount�Ĳ�����װ��һ�����в���
//	int* _useCount;
//	mutex* _mutex;
//};
//
//void testshard(){
//	sharedPtr<Date> sp(new Date);
//
//}
//
//
//template<class T>
//struct ListNode1{
//	shared_ptr<ListNode<T>> _prev;
//	shared_ptr<ListNode<T>> _next;
//
//	~ListNode()
//	{
//		cout << "~ListNode()" << endl;
//	}
//};


//template<class T>
//struct ListNode {
//	// ����weak_ptr���ü�����������
//	// weak_ptr������ֱ��ʹ�ã���Ҫ��shared_ptr���г�ʼ����ר�Ž�����ⳡ����shared_ptrѭ��Ӧ������
//
//	weak_ptr<ListNode <T>> _prev;
//	weak_ptr<ListNode <T>> _next;
//
//	~ListNode()
//	{
//		cout << "ListNode" << endl;
//	}
//};
//
//
//// ѭ�����ò��������������������Ա��������Ҫ�ȵ��ñ��������
//// ��˽�㲢�����ͷ�
//
//void t5(){
//	shared_ptr<ListNode<int>> sp(new ListNode<int>);
//	shared_ptr<ListNode<int>> sp2(new ListNode<int>);
//
//	cout << sp.use_count() << endl;
//	cout << sp2.use_count() << endl;
//
//	sp->_next = sp2;
//	sp2->_prev = sp;
//
//	cout << sp.use_count() << endl;
//	cout << sp2.use_count() << endl;
//}
//int main(){
//	t5();
//	//testshard();
//
//	system("pause");
//	return 0;
//}
// �쳣���ֵ���ȷд��
// ����һ�������Ӧһ�������
// �û������

//void t5(){
//	try{
//		vector<int> vec;
//		vec.resize(0xffffffff); //vector<T> too long
//		vec.at(0); // invalid vector<T> subscript
//	}
//	catch (exception& e)
//	{
//		cout << e.what() << endl;
//	}
//	catch (...)
//	{
//		cout << "δ֪�쳣" << endl;
//	}
//}

// ����ָ�룬���ܵĹ���ָ��
// ��̬���ٵĿռ����Ҫ�ͷţ����ͷž����ڴ�й©����

//void tests(){
//	//���򲻼���ڴ���Σ��
//	//�ó��ռ䵫��ʹ��
//
//	//0x743B49C2 ��(λ�� ����ָ��.exe ��)��δ��������쳣: 
//	// Microsoft C++ �쳣: std::bad_alloc��λ���ڴ�λ�� 0x00AFFBE8 ����
//	for (;;)
//		int* ptr = new int[100];
//}
//
//// RAII˼�룺���ݶ�����������ڿ�����Դ����������
//// RAIIһ��Ӧ�ã�����ָ��-->��������ָ��������������ڿ��ƶ�̬�ڶ��Ͽ����ڿռ�
//template<class T>
//class smartPtr{
//public:
//	smartPtr(T* ptr)
//		:_ptr(ptr)
//	{
//		cout << "��������ָ��" << endl;
//	}
//
//	~smartPtr()
//	{
//		cout << "����ָ�����������" << endl;
//		delete _ptr; 
//	}
//
//private:
//	T* _ptr;
//};
//void ft(){
//
//	while (1){
//		int* ptr = new int[100];
//		smartPtr<int> sp(ptr);
//		//throw 1;
//		//continue;
//		//cout << "delete" << endl;
//		//delete[] ptr;
//	}
//}
//
//void test(){
//	try{
//		ft();
//	}
//	catch (...){
//		cout << "test catch" << endl;
//	}
//}
//int main(){
//	test();
//	//t5();
//	system("pause");
//	return 0;
//}
//
////void t6(){
////	try{
////
////	}
////	catch (exception& e){
////		cout << e.what() << endl;
////	}
////	catch (...){
////		cout << "δ֪�쳣" << endl;
////	}
////}
//
//void t7(){
//	try
//	{
//		
//	}
//	catch (exception& e)
//	{
//		cout << e.what() << endl;
//	}
//	catch (...)
//	{
//		cout << "δ֪�쳣" << endl;
//	}
//}





//int main(){
//	//try{
//	//	f3();
//	//}
//	//catch (int a){
//	//	cout << "f3() int" << endl;
//	//}
//	//try{
//	//	f2();
//	//}
//	//catch (...)
//	//{
//	//	cout << "f2() throw(int, float)" << endl;
//	//}
//	//try{
//	//	test3();
//	//}
//	//catch (...)
//	//{
//	//	cout << "test3()" << endl;
//	//}
//	//test();
//	//try{
//	//	throw 1;
//	//}
//	//catch (int a){
//	//	cout << "throw 1" << endl;
//	//}
//	system("pause");
//	return 0;
//}



//int func(){
//	// throw���׳��쳣
//	throw 1;
//	return 1;
//}
//void test1(){
//	try
//	{
//		// try:���ܷ����쳣�Ĵ���飬д��try�����
//		func();
//	}
//	// ��������쳣
//	catch (int a)
//	{
//		cout << "func(int a)" << endl;
//	}
//	catch (char ch)
//	{
//
//	}
//	catch (char* pch)
//	{
//
//	}
//	catch (...)
//	{
//
//	}
//}
//
//void f1(){
//	throw 1;
//}
//// Ƕ��catch�����ͽ�ԭ��
//void test(){
//	try{
//
//	}
//	catch (int a)
//	{
//
//	}
//	// �������쳣���Ŵ����쳣֮��Ĳ���
//	cout << "try  catch" << endl;
//}
//
//// �ڴ��뾡����Ҫ������ԭ���¾�����
//// ���ã����쳣����Ҫ����
//// �쳣��ջչ��
//
//// �쳣�������׳�����һЩ���õ��¼�������ڴ�й©����
//
//void ff1(){
//	throw 1;
//}
//
//void test3(){
//
//	int* ptr = new int[1000];
//
//	try{
//		ff1();
//	}
//	catch (char ch)
//	{
//
//	}
//	catch (char* str)
//	{
//
//	}
//	catch (...)
//	{
//		// �����׳�֮ǰ����ɱ�Ҫ�Ĳ���
//		cout << "catch(...) delete" << endl;
//		delete[] ptr;
//		throw;
//	}
//	cout << "test3 delete" << endl;
//	delete[] ptr;
//}
//// �������͹��캯���о�����Ҫд�쳣���룬���׳����쳣����
//
//
//void f2() throw(int, float){
//	throw 1;
//}

//void f3() throw(...){
//
//	throw 1;
//	return;
//}


/*
//unsigned long sum = 0L;

// �� sum ��װ��ԭ������
// ��ʱ����++�Ĳ������ᱻ���
// �ڴ˲��ü����������Ĵ��۱Ƚϸ�
atomic<int> sum(0);
void fun(size_t num){
	for (size_t i = 0; i < num; ++i){
		++sum;
	}
}


template<class T>
struct Atomic{

};

struct number{
	int _num;
	number& operator++(){
		++_num;
		return *this;
	}
};

number n;

void fun1(size_t num){
	for (size_t i = 0; i < num; ++i){
		sum++;
	}
}
void fun2(size_t num){
	for (size_t i = 0; i < num; ++i){
		++n;
	}
}

void test1(){
	int num;
	cin >> num;

	thread t1(fun2, num);
	thread t2(fun2, num);

	t1.join();
	t2.join();

	cout << sum << endl;

	// atomic��������
	//atomic<int> sum2 = sum;
	//atomic<int> sum3(sum);
}
//��ͬ���߳���������ͬһ����
mutex mtx;
int global = 0;
void fun3(int num){
	for (size_t i = 0; i < num; ++i){

		// ������������������������߳�û���ͷŵ�ǰ��������ֱ�ӷ��ؼ���ʧ�ܵĽ��
		//mtx.try_lock(); //���Խ��м�������������
		// ����������������������߳�û���ͷŵ�ǰ�����������ȴ���ֱ�������߳��ͷ�
		// ������
		mtx.lock();
		//ͬһ�����������μ����Ͳ�������
		//mtx.lock();
// �����ǰ�߳�ӵ�и�����ִ�еڶ��μ����������ͻ�����

		
		++global;
		mtx.unlock();
	}
}
void test(){
	int num;
	cin >> num;

	thread t1(fun3, num);
	thread t2(fun3, num);

	t1.join();
	t2.join();

	cout << global << endl;
}

//timed_mutex tmtx;
//
//void test1(){
//	//tmtx.try_lock_for();
//}

//������
//�����ͽ����Ĺ��̲����ֶ����ƣ�ͨ��RAII�Զ�����

template<class Mutex>
struct GuardLock{
public:
	//���캯������
	GuardLock(Mutex& mtx)
		:_mutex(mtx)
	{
		cout << "GuardLock(Mutex& mtx)   g_n = "<<g_n << endl;
		_mutex.lock();
	}

	~GuardLock()
	{
		cout << "~GuardLock()   g_n = " << g_n << endl;
		_mutex.unlock();
	}

	//������
	//�������μ���ʱ������
	GuardLock(const GuardLock<Mutex>&) = delete;
	GuardLock& operator=(const GuardLock<Mutex>&) = delete;
private:
	Mutex& _mutex;
};

int g_n = 0;
void fun5(int num){
	for (int i = 0; i < num; i++){
		GuardLock<mutex> lg(mtx);
		++g_n; // ++����������ʵ�ַ��������ڴ�ȡ������ALU���㣬�Ż��ڴ棬����һ�����̶����ܻᱻ���
	}
}
void test2(){
	thread t1(fun5, 10);
	thread t2(fun5, 10);
	thread t3(fun5, 10);

	t1.join();
	t2.join();
	t3.join();

	cout << g_n << endl;
}
int main(){
	test2();
	//test();

	system("pause");
	return 0;
}
*/
// Ψһ������ȻΪRAII˼��

/*
void test(){
	try
	{
		vector<float> vf;
		// invalid vector<T> subscript
		//vf.at(10) = 2.0;

		// too long
		//vf.resize(0x3fffffff);//1G��Ԫ��λ��
		// bad allocation
	}
	// �������쳣�ĸ���������û���ָ��
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
	catch (...)
	{
		cout << "δ֪�쳣" << endl;
	}
}

void testSP(){
	//���ϵĳ��ڴ棬����ᱻ����
	
	int* ptr = new int(10);
	cout << *ptr << endl;

}


// ����ָ������ڴ�й©
template<class T>
struct smartPtr{
public:
	smartPtr(T* ptr)
		:_ptr(ptr)
	{

	}
private:
	T* _ptr;
};

*/
/*

// �쳣����

int func()
{
	// �׳������쳣�ͽ��������쳣����
	throw 0;//���쳣
	return 1;
}

class A{};

void af(){
	throw A();
}
void test1(){

	//Ƕ��catch��ʹ�þͽ�ԭ��ƥ��
	try
	{
		// ���ܷ����쳣�Ĵ���
		func();
	}
	// �����쳣
	catch (char ch)
	{
		cout << "catch(ch)" << endl;
	}
	catch (int a)//a��catch����鴦����ɾ�����
		// ����ʽ�߼�
	{
		cout << "catch(int) 1" << endl;
	}
	catch (exception e)
	{

	}
	// ȫ�䣬�����ظ�����
	// ... :һ���������catch����󣬲������еĲ��ɹ�
	catch
		(...)
	{

	}
}

void test2(){
	try
	{
		//func();
		test1();
	}
	catch (int n)
	{
		cout << "catch(int) 2" << endl;
	}
}
// ��������ף����������޷�������׳��쳣
void test3(){
	try
	{
		af();
	}
	catch (A  a)
	{
		cout << "A a" << endl;
	}
}

void test4(){
	try
	{

	}
	catch (...)
	{
		//�˴��������쳣�����ǽ�����ˣ���˽�������һ�����д���
		throw;
	}
}

void test5(){

	int *ptr = new int[1000];
	try{

	}
	catch (char ch)
	{
		//throw;
	}
	catch (...)
	{
		// �����׳�֮ǰ����ɱ�Ҫ�Ĳ���
		//�����׳�֮ǰ�����ͷſռ�
		cout << "catch(...) delete" << endl;
		delete[] ptr;
		throw; //�׳�֮��Ͳ���������������
	}

	cout << "out delete" << endl;
	delete[] ptr;
}

// һ���쳣�����ɼ̳���Ϣ��
// ���������Ϣ


void test1(){
	atomic<long> a(10);
	cout << a << endl;
}

mutex mtx;

void fun1(int num){
	for (int i = 0; i < num; ++i){

		mtx.try_lock();//������������������������߳�û���ͷŵ�ǰ��������ֱ�ӷ��ؼ���ʧ��
		//mtx.lock();//����������������������߳�û���ͷŵ�ǰ�����������ȴ���ֱ�������߳��ͷ���Ϊֹ
		//���������������������������
		// ͬһ�������ܽ������μ���
		// �����ǰû��ӵ��
	
		//�ݹ���
		mtx.unlock();
	}
}
// �쳣

void test(){

	int a = 10; 
	int&& ra = std::move(a);//��ֵ����
	//���治���õ��Ϳ������ƶ����壬��Ϊ��ֵ������
	// ����Ͳ�����ʹ�����������

}
// ����ָ��

// ������
template<class Mutex>
class lockGuard{
public:
	//���캯���н��м���
	lockGuard(Mutex& mtx)
		:_mtx(mtx)
	{
		cout << "lockGuard(Mutex&)" << endl;
		_mtx.lock();
	}
	//������������
	~lockGuard()
	{
		cout << "~lockGuard" << endl;
		_mtx.unlock();
	}
	// ��ֹ��������ʽ����
	//������������ֵ����delete
	lockGuard(const lockGuard<Mutex>&) = delete;
	lockGuard& operator=(const lockGuard<Mutex>&) = delete;
private:
	//����֧�ֿ���
	Mutex& _mtx;
};
*/
//int main(){
//	testSP();
//	//test();
//	//test3();
//	//test2();
//	//test1();
//	//test();
//
//
//
//	system("pause");
//	return 0;
//}
