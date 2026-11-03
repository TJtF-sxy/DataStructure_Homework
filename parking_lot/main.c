#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static int max_contain;
static double price;

typedef struct
{
  int id;
  int parked_time;
}Car;

typedef struct 
{
  Car *data;
  int top;
}Carpark_stack;

typedef struct Path//便道
{
  Car data;
  struct Path *next;
}Path, *PathLine;

typedef struct
{
  Path *front;
  Path *rear;
}Quene;

typedef struct
{
  int id;
  double price;
}Left_info;

bool IsStackFull(Carpark_stack *s) {
    return s->top == max_contain;
}

bool IsStackEmpty(Carpark_stack *s) {
    return s->top == 0;
}

bool Push(Carpark_stack *s, Car val) {
    if (IsStackFull(s)) {
        printf("栈满了，入栈失败！\n");
        return false;
    }
    s->data[s->top] = val; // 放入数据
    s->top++;              // 栈顶上移
    return true;
}

bool Pop(Carpark_stack *s, Car *val) {
    if (IsStackEmpty(s)) {
        printf("栈空了，出栈失败！\n");
        return false;
    }
    s->top--;              // 栈顶下移
    *val = s->data[s->top]; // 取出数据
    return true;
}

void InitQueue(Quene *q) {
    q->front = NULL;
    q->rear = NULL;
}

void EnQueue(Quene *q, Car car) {
    Path *newNode = (Path *)malloc(sizeof(Path));
    newNode->data = car;
    newNode->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

bool DeQueue(Quene *q, Car *car) {
    if (q->front == NULL) return false;
    Path *temp = q->front;
    *car = temp->data;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(temp);
    return true;
}

int main()
{
  scanf("%d%lf", &max_contain, &price);
  Car Carpark[max_contain];
  Car Carbuffer[max_contain];

  Carpark_stack Cpk, Cbf;
  Cpk.data = Carpark;
  Cpk.top = 0;
  Cbf.data = Carbuffer;
  Cbf.top = 0;

  Quene buffer_queue; 
  InitQueue(&buffer_queue);
  int func_id = 0;
  Left_info bill[100];
  int bill_idx = 0;
  while(1)
  {
    Car new_car;
    scanf("%d%d%d", &func_id, &new_car.id, &new_car.parked_time);

    if(func_id == 0 && new_car.id == 0 && new_car.parked_time == 0)
    {break;}

    if(func_id == 0)
    {
      if(IsStackFull(&Cpk))
      {
        EnQueue(&buffer_queue, new_car);
      }
      else
      {
        Push(&Cpk, new_car);
      }
    }
    else if(func_id == 1)
    {
      if(IsStackEmpty(&Cpk))
      {
        printf("no cars to pop\n");
      }
      int car_idx = -1;
      for(int i = 0; i < Cpk.top; i++)
      {
        if(Cpk.data[i].id == new_car.id)
        {
          car_idx = i;
          break;
        }
        //printf("car not found");
      }
      if(car_idx != -1)
      {
        for(int i = 0; i < Cpk.top - car_idx - 1; i++)
        {
          Car temp;
          Pop(&Cpk, &temp);
          Push(&Cbf, temp);
        }

        Car leave_car;
        Pop(&Cpk, &leave_car);

        bill[bill_idx].id = new_car.id;
        bill[bill_idx].price = (new_car.parked_time - Cpk.data[car_idx].parked_time) * price;
        bill_idx++;

        while(!IsStackEmpty(&Cbf))
        {
           Car temp;
           Pop(&Cbf, &temp);
           Push(&Cpk, temp);
        }

        while(!IsStackFull(&Cpk) && buffer_queue.front != NULL)
        {
          Car temp;
          DeQueue(&buffer_queue, &temp);
          Push(&Cpk, temp);
        }
      }
      else
      {
        if(buffer_queue.front == NULL)
        {
          printf("no car found");
        }
        else
        {
          int quene_len = 0;
          Path *p = buffer_queue.front;
          while(p != NULL)
          {
            quene_len++;
            p = p->next;
          }

          bool found = false;
          for(int i = 0; i < quene_len; i++)
          {
            Car temp;
            DeQueue(&buffer_queue, &temp);

            if(temp.id == new_car.id)
            {
              found = true;
              break;
            }
            else
            {
              EnQueue(&buffer_queue, temp);
            }
          }
          if(!found)
          {
            printf("car not found");
          }
        }
      }
    }
    else
    {
      printf("unknown command");
    }
  }
  for(int i = 0; i < bill_idx; i++)
  {
    printf("%d %.2lf\n", bill[i].id, bill[i].price);
  }
  for(int i = 0; i < Cpk.top; i++)
  {
    printf("%d ", Cpk.data[i].id);
  }
  printf("\n");
  while(buffer_queue.front != NULL)
  {
    Car temp;
    DeQueue(&buffer_queue, &temp);
    printf("%d ", temp.id);
  }
}