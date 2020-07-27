#define _CRT_SECURE_NO_WARNINGS 1
#include <memory>
#include <iostream>
#include <vector>
#include <mutex>
#include <atomic> // 原子操作
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
		//析构：引用计数自减
		if (subRef() == 0){
			delete _ptr;
			delete _useCount;
			delete _mutex;
			cout << "~sharedPtr" << endl;
		}
		//判断引用计数
	}

	sharedPtr(const sharedPtr<T>& sp)
		:_ptr(sp._sp)
		, _useCount(sp._useCount)
		, _mutex(sp._mutex)
	{
		addRef();
	}

	sharedPtr<T>& operator=(const sharedPtr<T> sp){
		//if (this != &sp) //只有管理的资源不同才表示不同的
		if (_ptr!=sp._ptr)
		{
			//判断当前管理的资源是否需要释放
			if (--(*_useCount) == 0){
				delete _ptr;
				delete _useCount;
				delete _mutex;
			}
			//赋值
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
	// 需要将_useCount的操作封装成一个穿行操作
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
	// 调用具体的析构函数对申请的空间进行修改
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
// shared_ptr第二个参数定制删器
void test(){
	freeM<int> fm;
	shared_ptr<int> sp(new int, fm);
}
int main(){
	test();


	system("pause");
	return 0;
}

// 连续加锁会造成死锁
// 递归函数：递归锁
// 守卫锁：RAII思想，防止未释放


// 智能指针： 
// 1、实现RAII思想
// 2、实现指针的功能

//template<class T>
//class smartPtr{
//public:
//	smartPtr(T* ptr)
//		:_ptr(ptr)
//	{
//		cout << "smartPtr(T* ptr)" << endl;
//	}
//
//	// 管理权转移
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
//			// 释放原有资源
//			if (_ptr)
//			{
//				delete _ptr;
//			}
//			// 管理权转移
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
//	smartPtr<int> sp(ptr); //并不是立即初始化
//	// 两个智能指针管理会造成二次释放的问题
//	//smartPtr<int> sp2(ptr);
//
//	//立即初始化
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
//	// 立即初始化
//	//smartPtr<Date> sp(new Date);
//	//// 两次释放
//	//smartPtr<Date> copy = sp;
//
//	Date* ptr = new Date;
//	Date* ptr2 = ptr;
//
//	delete ptr;
//}
//
//
//// 管理权转移
//void test5(){
//	// 一般是不用：有解引用异常的风险 
//	auto_ptr<Date> ap(new Date);
//	ap->_year = 2020;
//	cout << "ap->_year = " << ap->_year << endl;
//	// 拷贝时发生管理权转移
//	// 发生转移后，被转移的指针不能再使用
//	auto_ptr<Date> copy = ap;
//	// 发生管理权转移之后，智能指针不再拥有资源，故不能访问资源
//	cout << "copy->_year = "<< copy->_year << endl;
//	//cout << "ap->_year = " << ap->_year << endl;
//}
//
//void test6(){
//	unique_ptr<Date> up(new Date);
//	// unique_ptr :防拷贝-->拷贝构造和赋值运算符定义为delete函数
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
//	// 防拷贝
//	uniquePtr(const uniquePtr<T>& up) = delete;
//	uniquePtr<T>& operator = (const uniquePtr<T>& up) = delete;
//
//private:
//	T* _ptr;
//};
//
//// 加上引用计数：浅拷贝
//// 记录当前资源的智能指针管理个数为几个
//
////shared_ptr<class T> : 线程安全
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
//		//析构：引用计数自减
//		if (subRef() == 0){
//			delete _ptr;
//			delete _useCount;
//			delete _mutex;
//			cout << "~sharedPtr" << endl;
//		}
//		//判断引用计数
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
//		//if (this != &sp) //只有管理的资源不同才表示不同的
//		if (_ptr!=sp._ptr)
//		{
//			//判断当前管理的资源是否需要释放
//			if (--(*_useCount) == 0){
//				delete _ptr;
//				delete _useCount;
//				delete _mutex;
//			}
//			//赋值
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
//	// 需要将_useCount的操作封装成一个穿行操作
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
//	// 利用weak_ptr引用计数不会增加
//	// weak_ptr：不能直接使用，需要用shared_ptr进行初始化，专门解决特殊场景下shared_ptr循环应用问题
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
//// 循环引用不会调用析构函数：调成员的析构需要先调用本身的析构
//// 因此结点并不会释放
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
// 异常出现的正确写法
// 并非一个问题对应一个代码块
// 用基类接收

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
//		cout << "未知异常" << endl;
//	}
//}

// 智能指针，智能的管理指针
// 动态开辟的空间必须要释放，不释放就有内存泄漏问题

//void tests(){
//	//程序不间断内存最危险
//	//拿出空间但不使用
//
//	//0x743B49C2 处(位于 智能指针.exe 中)有未经处理的异常: 
//	// Microsoft C++ 异常: std::bad_alloc，位于内存位置 0x00AFFBE8 处。
//	for (;;)
//		int* ptr = new int[100];
//}
//
//// RAII思想：根据对象的生命周期控制资源在生命周期
//// RAII一个应用：智能指针-->根据智能指针对象在生命周期控制动态在堆上开辟在空间
//template<class T>
//class smartPtr{
//public:
//	smartPtr(T* ptr)
//		:_ptr(ptr)
//	{
//		cout << "构造智能指针" << endl;
//	}
//
//	~smartPtr()
//	{
//		cout << "智能指针的析构函数" << endl;
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
////		cout << "未知异常" << endl;
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
//		cout << "未知异常" << endl;
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
//	// throw：抛出异常
//	throw 1;
//	return 1;
//}
//void test1(){
//	try
//	{
//		// try:可能发生异常的代码块，写在try代码块
//		func();
//	}
//	// 捕获具体异常
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
//// 嵌套catch处理，就近原则
//void test(){
//	try{
//
//	}
//	catch (int a)
//	{
//
//	}
//	// 处理完异常接着处理异常之后的操作
//	cout << "try  catch" << endl;
//}
//
//// 在代码尽量不要崩溃的原则下尽量好
//// 重置，抛异常，不要闪退
//// 异常的栈展开
//
//// 异常的重新抛出，做一些有用的事件，解决内存泄漏问题
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
//		// 重新抛出之前，完成必要的操作
//		cout << "catch(...) delete" << endl;
//		delete[] ptr;
//		throw;
//	}
//	cout << "test3 delete" << endl;
//	delete[] ptr;
//}
//// 在析构和构造函数中尽量不要写异常代码，容易出现异常问题
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

// 将 sum 封装成原子类型
// 此时整个++的操作不会被打断
// 在此不用加锁，加锁的代价比较高
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

	// atomic：防拷贝
	//atomic<int> sum2 = sum;
	//atomic<int> sum3(sum);
}
//不同的线程争抢的是同一把锁
mutex mtx;
int global = 0;
void fun3(int num){
	for (size_t i = 0; i < num; ++i){

		// 非阻塞加锁操作：如果其他线程没有释放当前的锁，则直接返回加锁失败的结果
		//mtx.try_lock(); //尝试进行加锁，不会阻塞
		// 阻塞加锁操作：如果其他线程没有释放当前的锁，阻塞等待，直到其他线程释放
		// 会阻塞
		mtx.lock();
		//同一个锁进行两次加锁就产生死锁
		//mtx.lock();
// 如果当前线程拥有该锁，执行第二次加锁操作，就会死锁

		
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

//守卫锁
//加锁和解锁的过程不用手动控制，通过RAII自动控制

template<class Mutex>
struct GuardLock{
public:
	//构造函数加锁
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

	//防拷贝
	//进行两次加锁时防拷贝
	GuardLock(const GuardLock<Mutex>&) = delete;
	GuardLock& operator=(const GuardLock<Mutex>&) = delete;
private:
	Mutex& _mutex;
};

int g_n = 0;
void fun5(int num){
	for (int i = 0; i < num; i++){
		GuardLock<mutex> lg(mtx);
		++g_n; // ++：操作三步实现方法，从内存取出来，ALU运算，放回内存，任意一个过程都可能会被打断
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
// 唯一锁，仍然为RAII思想

/*
void test(){
	try
	{
		vector<float> vf;
		// invalid vector<T> subscript
		//vf.at(10) = 2.0;

		// too long
		//vf.resize(0x3fffffff);//1G个元素位置
		// bad allocation
	}
	// 用所有异常的根基类的引用或者指针
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
	catch (...)
	{
		cout << "未知异常" << endl;
	}
}

void testSP(){
	//不断的吃内存，迟早会被吃完
	
	int* ptr = new int(10);
	cout << *ptr << endl;

}


// 智能指针避免内存泄漏
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

// 异常处理

int func()
{
	// 抛出整形异常就进行整形异常处理
	throw 0;//抛异常
	return 1;
}

class A{};

void af(){
	throw A();
}
void test1(){

	//嵌套catch：使用就近原则匹配
	try
	{
		// 可能发生异常的代码
		func();
	}
	// 处理异常
	catch (char ch)
	{
		cout << "catch(ch)" << endl;
	}
	catch (int a)//a当catch代码块处理完成就销毁
		// 拷贝式逻辑
	{
		cout << "catch(int) 1" << endl;
	}
	catch (exception e)
	{

	}
	// 全配，不可重复捕获
	// ... :一般放在整个catch的最后，捕获所有的不成功
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
// 逐层往外抛，到主函数无法处理就抛出异常
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
		//此处捕获了异常，但是解决不了，因此交给了上一级进行处理
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
		// 重新抛出之前，完成必要的操作
		//重新抛出之前，先释放空间
		cout << "catch(...) delete" << endl;
		delete[] ptr;
		throw; //抛出之后就不会运行下面的语句
	}

	cout << "out delete" << endl;
	delete[] ptr;
}

// 一般异常都是由继承信息的
// 捕获基类信息


void test1(){
	atomic<long> a(10);
	cout << a << endl;
}

mutex mtx;

void fun1(int num){
	for (int i = 0; i < num; ++i){

		mtx.try_lock();//非阻塞加锁操作：如果其他线程没有释放当前的锁，则直接返回加锁失败
		//mtx.lock();//阻塞加锁操作：如果其他线程没有释放当前的锁，阻塞等待，直到其他线程释放锁为止
		//上锁不能两次上锁，否则会死锁
		// 同一个锁不能进行两次加锁
		// 如果当前没有拥有
	
		//递归锁
		mtx.unlock();
	}
}
// 异常

void test(){

	int a = 10; 
	int&& ra = std::move(a);//右值语义
	//后面不再用到就可以用移动语义，变为右值的属性
	// 后面就不能再使用这个变量了

}
// 智能指针

// 守卫锁
template<class Mutex>
class lockGuard{
public:
	//构造函数中进行加锁
	lockGuard(Mutex& mtx)
		:_mtx(mtx)
	{
		cout << "lockGuard(Mutex&)" << endl;
		_mtx.lock();
	}
	//析构函数解锁
	~lockGuard()
	{
		cout << "~lockGuard" << endl;
		_mtx.unlock();
	}
	// 防止拷贝，方式加锁
	//防拷贝，防赋值利用delete
	lockGuard(const lockGuard<Mutex>&) = delete;
	lockGuard& operator=(const lockGuard<Mutex>&) = delete;
private:
	//锁不支持拷贝
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
