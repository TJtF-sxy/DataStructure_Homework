#include <stdio.h>
#include <time.h>
#include <stdlib.h>

typedef struct Node
{
    int id;
    int code;
    struct Node *next;
}Node, *List;

int main()
{
    int n, m;
    scanf("%d%d", &m, &n);
    int kico_out_order[n];
    int order_index = 0;
    int call_number =  1;
    //srand(time(NULL));

    //Init List
    List L;
    L = (List)malloc(sizeof(Node));
    L->id = 0;
    L->code = 0;
    Node* temp = L;

    /**按照输入的链结数量生成链结，同时每次手动输入链结的密码 */
    for(int i = 1; i <= n; i++)
    {
        Node* newNode = (Node *)malloc(sizeof(Node));
        newNode->id = i ;
        scanf("%d", &newNode->code);
        //newNode->code = rand() % m + 1;
        temp->next = newNode;
        temp = newNode;
    }

    //Kick out
    /**将最后的链结连上头结点，形成环形链表 */
    temp->next = L; // Make it circular
    Node* current = L;
    temp = L->next;

    /**判定条件：如果call_number小于m，则直接跳过进入下一次while循环
     * 如果call_number等于m，则将当前链结踢出，并将call_number重置为1
     * 踢出方式：将当前链结的前一个链结的next指针指向当前链结的下一个链结，并释放当前链结的内存
     * 注意：如果当前链结为L，则需要将temp指向L->next，并将current指向L，以此来避免将L释放导致内存泄漏
     */
    while(L->next != L)
    {
        if(call_number < m)
        {
            current = temp;
            if(temp->next == L)
            {
              temp = L->next;
              current = L;
            }
            else
            {
                temp = temp->next;
            }
            call_number++;
            continue;
        }
        kico_out_order[order_index] = temp->id;
        m = temp->code;
        current->next = temp->next;
        free(temp);
        temp = current->next;
        if(temp == L)
        {
            temp = L->next;
            current = L;
        }
        order_index++;
        call_number = 1;
    }

    for(int i = 0; i < order_index; i++)
    {
        printf("%d ", kico_out_order[i]);
    }
    printf("\n");
    return 0;
}
