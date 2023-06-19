#include <iostream>
#include "windows.h"
using namespace std;

HANDLE work_event, sum_event;
char* arr;
int k;
double cnt = 0;
CRITICAL_SECTION cs;

DWORD WINAPI Sort(LPVOID  mas) {
    int sleep_time = 0;
    cout << "Enter a sleeping time: ";
    cin >> sleep_time;
    int array_size = _msize(arr) / sizeof(char);
    char *min = &arr[0];
    for (int repeat_counter = 0; repeat_counter < array_size; repeat_counter++)
    {
        int temp = arr[0];
        for (int element_counter = repeat_counter + 1; element_counter < array_size; element_counter++)
        {
            if (arr[repeat_counter] > arr[element_counter])
            {
                temp = arr[repeat_counter];
                arr[repeat_counter] = arr[element_counter];
                arr[element_counter] = temp;
            }
        }
        Sleep(sleep_time);
        if (repeat_counter == k)
        {
            SetEvent(work_event);
        }
    }
    
    cout << endl;
    return 0;
}

DWORD WINAPI SumElement(LPVOID  mas) {
    EnterCriticalSection(&cs);
    DWORD dwWaitResult = WaitForSingleObject(sum_event, INFINITE);
    if (dwWaitResult == WAIT_OBJECT_0)
    {
        int array_size = _msize(arr) / sizeof(char);
        for (int i = 0; i < k; i++) {
            cnt += arr[k];
        }
        cnt /= k;
        LeaveCriticalSection(&cs);
    }
    return 0;
}


int main() {

    InitializeCriticalSection(&cs);
    int array_size = 0;
    cout << "Enter size of array: ";
    cin >> array_size;
    arr = new char[array_size];
    for (int i = 0; i < array_size; i++) {
        cout << "Enter element [" << i << "]\n";
        cin >> arr[i];
    }
    cout << "arrays's size: " << array_size << endl;
    for (int i = 0; i < array_size; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
    DWORD workerid;
    work_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    sum_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    cout << "Enter k: ";
    cin >> k;
    HANDLE worker = CreateThread(NULL, 0, &Sort, NULL, NULL, &workerid);
    DWORD sum_element_id;
    HANDLE sum_element = CreateThread(NULL, 0, &SumElement, NULL, NULL, &sum_element_id);
    if (worker == NULL || sum_element == NULL) {
        return GetLastError();
    }
    WaitForSingleObject(work_event, INFINITE);
    cout << "Resulting arr from main thread: " << endl;
    for (int i = 0; i < k; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
    SetEvent(sum_event);
    cout << "Resulting arr to an element with index k from main thread: ";
    EnterCriticalSection(&cs);

    cout << endl;
    cout << "Resulting cnt: " << cnt << endl;
    LeaveCriticalSection(&cs);
    DeleteCriticalSection(&cs);
    WaitForSingleObject(worker, INFINITE);
    CloseHandle(worker);
    CloseHandle(sum_element);
    CloseHandle(work_event);
    CloseHandle(sum_event);

    for (int i = k + 1; i < array_size; i++) {
        cout << arr[i] << " ";
    }
    return 0;
   
}
