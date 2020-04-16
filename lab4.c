#include "stdio.h"
#include "stdlib.h"
#include <string.h>

#define maxBuff 1024
#define NULL_MEMORY  "Error with allocating memory"
#define NULL_LINE  "Error with reading string"

typedef struct node
{
    char *data;
    struct node *next;
} list;

int main()
{
    char *line = (char *) malloc(sizeof(char) * maxBuff);

    if (line == NULL)
    {
        perror(NULL_MEMORY);
        return 1;
    }

    list *head, *cur, *temp;
    head = (list *) malloc(sizeof(list));

    if (head == NULL)
    {
        perror(NULL_MEMORY);
        return 1;
    }

    head->next = NULL;
    cur = head;

    while (1)
    {
        if (fgets(line, maxBuff, stdin) == NULL)
        {
            perror(NULL_LINE);
            return 1;
        }
        if (line[0] == '.')
            break;
        temp = (list *) malloc(sizeof(list));

        if (temp == NULL)
        {
            perror(NULL_MEMORY);
            return 1;
        }

        temp->data = (char *) malloc(sizeof(char) * strlen(line) + 1);

        if (temp->data == NULL)
        {
            perror(NULL_MEMORY);
            return 1;
        }
        strcpy(temp->data, line);
        temp->next = NULL;
        cur->next = temp;
        cur = cur->next;
    }

    temp = head->next;

    while (1)
    {
        printf("%s", temp->data);
        temp = temp->next;
        if (temp == NULL)
            break;
    }

    temp = head;
    while (1)
    {
        head = head->next;
        free(temp);
        if (head == NULL)
            break;
        temp = head;

    }

    free(line);

    return 0;
}
