#include <stdio.h>
#include <stdlib.h>

typedef struct day
{
    int id;
    double min;
    double max;
} day;

typedef struct daychain
{
    day day;
    struct daychain *next;
} daychain;

void print_my_days(daychain *head)
{
    puts("day   min        max");
    daychain *current = head;
    while (current)
    {
        printf("%d     %f   %f\n", current->day.id, current->day.min, current->day.max);
        current = current->next;
    }
}

daychain *add_day(daychain *head, day d)
{
    daychain *new_dc = (daychain *)malloc(sizeof(daychain));
    new_dc->day = d;
    new_dc->next = NULL;

    if (!head || head->day.id > d.id)
    {
        new_dc->next = head;
        return new_dc;
    }

    daychain *current = head;
    while (current->next && current->next->day.id < d.id)
    {
        current = current->next;
    }

    if (current->day.id == d.id)
    {
        current->day = d;
        free(new_dc);
    }
    else
    {
        new_dc->next = current->next;
        current->next = new_dc;
    }

    return head;
}

daychain *delete_day(daychain *head, int id)
{
    daychain *current = head, *prev = NULL;

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
            return head;
        }
        prev = current;
        current = current->next;
    }
    return head;
}

void free_list(daychain *head)
{
    while (head)
    {
        daychain *temp = head;
        head = head->next;
        free(temp);
    }
}

int main()
{
    daychain *head = NULL;
    char input[100];

    while (1)
    {
        printf("Enter command: ");
        fgets(input, 100, stdin);

        if (input[1] != ' ' && input[1] != '\n' && input[1] != '\r')
            goto BAD_INPUT;

        if (input[0] == 'A')
        {
            int id;
            double min, max;

            if (sscanf(input, "A %d %lf %lf", &id, &min, &max) != 3)
            {
                printf("Invalid input format\n");
                continue;
            }

            if (id < 1 || id > 31)
            {
                printf("Invalid ID\n");
                continue;
            }

            day d = {id, min, max};
            head = add_day(head, d);
        }
        else if (input[0] == 'D')
        {
            head = delete_day(head, atoi(&input[2]));
        }
        else if (input[0] == 'P')
        {
            print_my_days(head);
        }
        else if (input[0] == 'Q')
        {
            break;
        }
        else
        {
            goto BAD_INPUT;
        }
        continue;

    BAD_INPUT:
        puts("Invalid input");
    }

    free_list(head);
    return 0;
}
