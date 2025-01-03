#define _CRT_SECURE_NO_WARNINGS // Для совместимости со старыми c-style функциями обработки строк

#include <iostream>
#include <cstring>
#include <string>


// Пример функции с параметрами, передаваемыми по ссылке.
// Будет демонстрироваться в коде (PART 4).
int sum(int& a, int& b)
{
    a += 10;       // внимание! число а пришло по ссылке
    return a + b;
}


// Пример функции с параметром-массивом.
// Будет демонстрироваться в коде (PART 6).
void processArray(int a[], std::size_t len)
{
    // будет выводить 1, т.к. "int a[]" эквивалентно "int* a",
    // а размер указателя а равен 4 (как и размер инта) 
    // на данной архитектуре процессора (32 bit)
    std::cout << sizeof(a) / sizeof(int) << std::endl;
	
    // можно что-то сделать с массивом
    // ...
}

void printArray(int a[], std::size_t len)
{
    for (std::size_t i = 0; i < len; ++i)
    {
        std::cout << a[i] << " ";
    }
    std::cout << std::endl;
}


int main()
{
    // ***************************************
    // ===== PART 1. УКАЗАТЕЛИ И АДРЕСА ======
    // ***************************************

    int value = 45;
    double coeff = -113.29;

    // операция & - получить адрес переменной в памяти

    std::cout << &value << std::endl;	// 00 22 f7 98	 (адрес value, этот адрес может, естественно, отличаться в каждом конкретном случае)
    std::cout << &coeff << std::endl;	// 00 22 f7 90   (адрес переменной coeff в памяти)

    int* valuePtr = &value;		// адрес можно присвоить переменной типа "указатель" (int*): 00 22 f7 98
    double* coeffPtr = &coeff;

    // операция * - разыменовать указатель (получить те данные в памяти, на которые он указывает)
    std::cout << *valuePtr << std::endl;


	
    // ***************************************
    // =========== PART 2. ССЫЛКИ ============
    // ***************************************

    int& ref = value;	// алиас, псевдоним для переменной value
                        // в данном случае & означает "ссылку" (другое имя для переменной value)
    ref++;	        // "нарастили" ссылку

    std::cout << value << std::endl;	// а в итоге нарастилась также и та переменная, на которую ссылается ref
    value++;

    std::cout << ref << std::endl;	// ref и value ссылаются на одни и те же данные


	
    // ***************************************
    // ====== PART 3. УКАЗАТЕЛИ и Const ======
    // ***************************************

    int ten = 10, twenty = 20;

    const int* ptr1 = &ten;	// указатель на неизменяемое значение

    //(*ptr1)++;		// так сделать нельзя
    ptr1 = &value;		// а сам указатель изменить можно

    int* const ptr2 = &twenty;	// неизменямый (!) указатель (сам)

    (*ptr2)++;			// изменить то, на что он указывает, можно, а сам указатель - нельзя
    //ptr2 = &value;            // так сделать нельзя

    std::cout << twenty << std::endl;

    const int* const ptr3 = &ten;
    std::cout << *ptr3 << std::endl;



	
    // ***************************************
    // ====== PART 4. ПАРАМЕТРЫ ФУНКЦИЙ ======
    // ***************************************

    int x1 = 15;
    int x2 = 40;

    int res = sum(x1, x2); 	// передаем параметры по ссылке (см. сигнатуру функции sum()),
                            // поэтому х1 изменится в теле функции (т.к. там меняется ее ссылка - а).
                            // А вообще параметры функций копируются в локальные переменные функций
                            // и существуют только в теле функции
    std::cout << "x1 = " << x1 << std::endl;
    std::cout << "Sum = " << res << std::endl;

	

    // ***************************************
    // ======= PART 5. КЛАССЫ ПАМЯТИ =========
    // ***************************************

    // code	- memory where code instructions are stored
    // globals	- memory for all static variables and globals
    // STACK	- "fast" memory
    // HEAP	- "big" memory
	
    int inner = 90;	// это создано и живет в стеке

    // специально создадим блок {...} для демонстрации области действия переменных (scope)
    {
        int* innerPtr = &inner;	    // указатель на inner
        (*innerPtr)++;

        int* heapPtr = new int;	    // оператор new выделяет память в куче (HEAP) - в данном случае там выделилось 4 байта под int
        *heapPtr = 798;		    // и в эту ячейку записали значение 798
    }
                        // здесь оба указателя (хранившиеся в стеке) уже уничтожатся (по 4 байта),
                        // т.к. выйдут из области действия {...}

    inner++;		// с inner будет все в порядке, т.к. уничтожился только указатель на нее
    std::cout << inner << std::endl;

    // а кусок памяти, в котором хранится 798 будет бесполезно занимать место в куче,
    // т.к. единственный указатель на него уничтожен
    // это утечка памяти - memory leak
	
    int* val = new int;	// опять выделить память в HEAP под один int
    *val = 35;		// присвоить по указателю данные 35

    delete val;		// освободить в памяти HEAP (теперь утечки памяти не будет, когда в конце main() указатель val будет уничтожен)
    val = NULL; 	// delete освобождает память, но ничего не делает с указателем. Здесь сами делаем val = 0;

    // Кстати, здесь есть важнейший нюанс!
    // - мы можем спокойно удалить нулевой указатель:
    //   val = NULL;
    //   delete val;
    // Спонсор нашего спокойствия - стандарт С++, согласно которому компилятор просто проигнорирует delete.
    // Но (!) вызов delete два раза подряд для одного и того же ненулевого указателя
    // приводит к самым нежелательным буквам в мире С++ - UB (Undefined Behaviour), 
    // а конкретно в этом случае почти наверняка crash программы. Т.е. так делать нужно забыть:
    // 	 delete x;
    // 	 delete x;
	

    // ***************************************
    // ======== PART 6. МАССИВЫ (1D) =========
    // ***************************************

    // пример массива, объявленного статически (живет в стеке) и с инициализацией сразу
    int arr[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    std::cout << arr[0] << std::endl;	// возможен доступ по индексу - как обычно

    int* heapArray = new int [5];	// пример массива, объявленного динамически (живет в куче, а в стеке живет указатель на первый элемент)
	
    for (std::size_t i = 0; i < 5; ++i)	// пример другой инициализации - в цикле числами 42, 43, 44, 45, 46
    {                                   // NB. Тип std::size_t - в данном случае эквивалент unsigned int
        heapArray[i] = 42 + i;
    }

    std::cout << heapArray << std::endl;    // оказывается, массив в С++ - это суть не что иное, как указатель на первый элемент
    std::cout << *heapArray << std::endl;   // так мы получим первый элемент (разыменовав переменную, которая "отвечает" за массив)

    // так можно узнать размер массива на стеке
    std::cout << sizeof(arr) / sizeof(arr[0]) << std::endl;

    // передаем массив в функцию (передача параметра по указателю - поэтому массив будет меняться)
    processArray(arr, 10);


	
    // ***************************************
    // ==== PART 7. АРИФМЕТИКА УКАЗАТЕЛЕЙ ====
    // ***************************************

    int* pointer = arr;
    pointer++;		// нарастили указатель на 1.
                    // это не означает, что мы увеличили адрес ячейки памяти на 1.
                    // А означает, что мы продвинулись вперед на то количество байт, которое занимает тип при указателе (int*)
                    // Это т.н. арифметика указателей

    std::cout << *pointer << std::endl;	    // т.е. здесь будет второй элемент массива
                                            // (хотя указатель мы сдвинули на 1, а не на 4 (байта))
    short* shortArray = (short*)heapArray;	// а вот если попытаемся "шагать" по массиву 2-байтовыми шагами (short*) ...

    for (std::size_t i = 0; i < 5; ++i)
    {
        *(shortArray + i) = 42 + i;	    // (эта запись эквивалентна shortArray[i]
                                        // (оператор [] был введен в С++ просто для большей читабельности))
    }

    std::cout << *heapArray << std::endl;   // ... то здесь уже будет не то число, т.к. мы в 4-байтовой ячейке оказались два раза
                                            // в предыдущем цикле (шагая по 2 байта)
    delete []heapArray;




    // ***************************************
    // ======== PART 8. МАССИВЫ (2D) =========
    // ***************************************

    //int arr2D[2][6];			// пример двумерного массива 2х6, объявленного статически
	
    int** arr2D = new int* [2];		// пример двумерного массива 2х6, объявленного динамически
                                    // это по сути указатель на указатели (или массив массивов)
                                    // здесь мы создаем массив указателей (массив строк)
	
    for (std::size_t i = 0; i < 2; ++i)	// а здесь мы создаем массив по каждой строке массива
    {
        arr2D[i] = new int [6];
    }

    // ... делаем что-нибудь с массивом ...
    // например, присвоим последнему элементу значение 73
    arr2D[1][5] = 73;

    for (std::size_t i = 0; i < 2; ++i)  // так мы освобождаем память, выделенную под двумерный массив
    {
        delete [] arr2D[i];
    }
    delete [] arr2D;





    // ***************************************
    // =========== PART 9. СТРОКИ ============
    // ***************************************
    char text[100] = { 'H', 'e', 'l', 'l', 'o' };
    wchar_t widetext[100] = L"Hello!";

    std::cout << text << std::endl;
    std::wcout << widetext << std::endl;

    // Есть C-style функции для работы со строками, например, strcat():
    char newText[100] = " world!";
    strcat(text, newText);
    std::cout << text << std::endl;

    // В С++ есть удобный и стандартный строковый тип std::string
    std::string str = "Hello";
    std::wstring wstr = L"Hello";
    str.append(" world!");
    std::cout << str << std::endl;
    wstr.append(L" world!");
    std::wcout << wstr << std::endl;




    // ***************************************
    // ====== PART 10. УКАЗАТЕЛЬ VOID* =======
    // ***************************************

    // Прототип функции memset
    // std::memset(void* destination, int value, std::size_t num)

    int numbers[128];
    std::memset(numbers, 0, 128*sizeof(int));

    // Прототип функции memcpy
    // std::memcpy(void* destination, const void* src, std::size_t num)

    int coeffs[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    std::memcpy(coeffs, numbers, 5 * sizeof(int));

    printArray(coeffs, 10);

    // приведенные функции работают не только с массивами, а вообще
    // с любыми данными в памяти, т.к. в памяти это все наборы байт




    // ***************************************
    // ==== PART 11. УКАЗАТЕЛИ НА ФУНКЦИЮ ====
    // ***************************************

    void (*arrayFunc)(int[], std::size_t) = processArray;
    arrayFunc(arr, 10);

    arrayFunc = printArray;
    arrayFunc(arr, 10);

    // указатель arrayFunc имеет свойства указателей, а значит:
    // - его можно перебросить на другую функцию (с аналогичным прототипом)
    // - его можно передать в другую функцию в параметре
    // - можно сделать массив таких указателей и "прогонять" массив данных по всем функциям из него

    return 0;
}
