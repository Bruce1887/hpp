#include <stdio.h>
#include <stdlib.h>

typedef struct day day;
struct day
{
    int id;
    double min;
    double max;
};

typedef struct daychain daychain;
struct daychain
{
    day day;
    daychain *next;
};

daychain *head = NULL;

void print_my_days()
{
    puts("day   min        max");
    daychain *current = head;
    while (current)
    {
        printf("%d     %f   %f\n", current->day.id, current->day.min, current->day.max);
        current = current->next;
    }
}

void add_day(day d)
{
    daychain *new_dc = (daychain *)malloc(sizeof(daychain));
    new_dc->day = d;
    new_dc->next = NULL;

    if (head == NULL)
    {
        head = new_dc;
    }
    else
    {
        daychain *current = head;
        while (current->next)
        {
            if (current->day.id >= d.id)
            {
                break;
            }
            current = current->next;
        }
        if (current->day.id == d.id)
        {
            current->day = d;
            free(new_dc);
            return;
        }
        daychain *temp = current->next;
        current->next = new_dc;
        new_dc->next = temp;
    }
}

void delete_day(int id)
{
    daychain *current = head;
    daychain *prev = NULL;
    while (current)
    {
        if (current->day.id == id)
        {
            if (prev)
            {
                prev->next = current->next;
            }
            else
            {
                head = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

int main(int argc, char *argv[])
{
    char input[100];
    while (1)
    {
        printf("Enter command: ");
        fgets(input, 100, stdin);

        if (input[1] != ' ' && input[1] != '\n' && input[1] != '\r')
            goto BAD_INPUT;

        if (input[1] != ' ' && input[1] != '\n' && input[1] != '\r')
            printf("BAD input[1] = %d\n", input[1]);

        if (input[0] == 'A')
        {
            int id;
            double min, max;

            if (sscanf(input, "A %d %lf %lf", &id, &min, &max) != 3) {
                printf("Invalid input format\n");
                continue;
            }

            if (id < 1 || id > 31) {
                printf("Invalid ID\n");
                continue;
            }

            day d = {id, min, max}; 
            add_day(d);
        }
        else if (input[0] == 'D')
        {
            delete_day(atoi(&input[2]));
        }
        else if (input[0] == 'P')
        {
            print_my_days();
        }
        else if (input[0] == 'Q')
        {
            goto EXIT;
        }
        else
        {
            goto BAD_INPUT;
        }
        continue;
    BAD_INPUT:
        puts("Invalid input");
    }

EXIT:
    for (daychain *current = head; current;)
    {
        daychain *temp = current;
        current = current->next;
        free(temp);
    }
    return 0;
}