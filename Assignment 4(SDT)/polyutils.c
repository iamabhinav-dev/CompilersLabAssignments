#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "poly.h"

Treenode* headOfProduction ; 

// Helper function to calculate the power of a number
int binExp(int base, int exp)
{
    if (exp == 0)
        return 1;
    int half = binExp(base, exp / 2);
    if (exp % 2)
        return half * half * base;
    else
        return half * half;
}
// Helper function to convert an integer to a string
char *intToStr(int value)
{
    // Custom function to convert an integer to a string
    char *str = malloc(12); // Enough for int32_t max value
    if (str)
    {
        sprintf(str, "%d", value);
    }
    return str;
}
// Helper function to concatenate two strings
char *concat(char *str1, char *str2)
{
    if (!str1)
        return strdup(str2); // If str1 is NULL, return a duplicate of str2
    if (!str2)
        return strdup(str1); // If str2 is NULL, return a duplicate of str1

    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    char *result = malloc(len1 + len2 + 1); // Allocate memory for the new string
    if (result)
    {
        strcpy(result, str1); // Copy str1 into result
        strcat(result, str2); // Concatenate str2 onto result
    }
    return result; // Return the new concatenated string
}
// Function to create a new node
Treenode *createNode(char type)
{
    Treenode *temp = (Treenode *)malloc(sizeof(Treenode));
    temp->type = type;
    temp->child = NULL;
    temp->sibling = NULL;
    temp->inh = NULL;
    return temp;
}
// Function to add a child to a Treenode
void addChild(Treenode *parent, Treenode *child)
{
    if (parent->child == NULL)
    {
        parent->child = child; // If no child, add the first child
    }
    else
    {
        // If there is already a child, add to the sibling list
        Treenode *sibling = parent->child;
        while (sibling->sibling != NULL)
        {
            sibling = sibling->sibling; // Traverse to the last sibling
        }
        sibling->sibling = child; // Add as the last sibling
    }
    // Debug print
    // printf("Added child: Type: %c, Val: %s, Parent Type: %c\n", child->type, child->val, parent->type);
}
// Function to free the entire tree
void freeTree(Treenode *root)
{
    if (root == NULL)
        return;

    // Recursively free child and sibling nodes
    freeTree(root->child);
    freeTree(root->sibling);

    // Free the Treenode itself
    free(root);
}
// Helper function to print the indentation
void printIndent(int levels[], int level)
{
    for (int i = 0; i < level; i++)
    {
        if (levels[i])
        {
            printf("|  ");
        }
        else
        {
            printf("   ");
        }
    }
}
// Function to print the tree with proper indentation (and all attributes)
void printTree(Treenode *root, int level, int isLast)
{
    if (root == NULL)
        return;

    // Print indentation based on level
    for (int i = 0; i < level - 1; i++)
    {
        printf("|   "); // Print vertical line for previous levels
    }

    // Print branch
    if (level > 0)
    {
        if (isLast)
        {
            printf("└── "); // Last child at this level
        }
        else
        {
            printf("├── "); // Not the last child
        }
    }

    // Print the current Treenode's type
    printf("%c", root->type);

    // Print attributes inside a single square bracket
    printf(" [");

    // If both val and inh are NULL, print empty brackets
    if (root->val == NULL && root->inh == NULL)
    {
        // Nothing to print, just print the closing bracket
    }
    else
    {
        // Print Val if it is not NULL
        if (root->val != NULL)
        {
            printf("Val: %s", root->val);
        }

        // Print Inh if it is not NULL, add comma if Val was also printed
        if (root->inh != NULL)
        {
            if (root->val != NULL)
            {
                printf(", ");
            }
            printf("Inh: %s", root->inh);
        }
    }

    // Close the square bracket
    printf("]\n");

    // Recursively print the children of the current Treenode
    Treenode *child = root->child;
    while (child != NULL)
    {
        // Check if the current child is the last one
        Treenode *sibling = child->sibling;
        int isLastChild = (sibling == NULL);      // No more siblings
        printTree(child, level + 1, isLastChild); // Recur for child nodes
        child = sibling;                          // Move to the next sibling
    }
}

// Function to set attributes for all nodes in the tree
void setatt(Treenode *node, Treenode *parent)
{
    if (node == NULL)
        return;
    //     if (node != NULL) {
    //     printf("Node type: %c, Child: %p, Sibling: %p\n", node->type, (void*)node->child, (void*)node->sibling);
    // }

    // First, set attributes for the current node based on its type and parent
    switch (node->type)
    {
    case 'S':
        // The root node doesn't need any attribute set
        break;
    case 'P':
        if (parent && parent->type == 'S')
        {
            // If parent is S, inherit sign from the preceding + or -
            Treenode *sign = parent->child;
            if (sign->type == '-')
            {
                node->inh = strdup("-");
            }
            else
            {
                node->inh = strdup("+");
            }
        }
        else if (parent && parent->type == 'P')
        {
            // If parent is P, inherit sign from the preceding + or -
            Treenode *sibling = parent->child;
            while (sibling != node && sibling != NULL)
            {
                if (sibling->type == '+' || sibling->type == '-')
                {
                    node->inh = strdup(sibling->type == '+' ? "+" : "-");
                }
                sibling = sibling->sibling;
            }
        }
        else
        {
            // Default case
            node->inh = strdup("+");
        }
        break;
    case 'T':
        // Inherit sign from parent P
        if (parent && parent->type == 'P')
        {
            node->inh = strdup(parent->inh);
        }
        else
        {
            node->inh = strdup("+"); // Default case
        }
        break;
    case 'N':
        setatt(node->child, node);
        if (node->child->sibling == NULL)
        {
            // N → D
            node->val = strdup(node->child->val);
        }
        else
        {
            // N → 1 M or N → D M
            Treenode *firstChild = node->child;
            Treenode *secondChild = firstChild->sibling;
            setatt(secondChild, node);
            node->val = strdup(secondChild->val);
        }
        break;
    case 'M':
        if (node->child->sibling == NULL)
        {//M->0|1|D
            setatt(node->child, node);
            node->inh = concat(parent->inh, parent->child->val);
            node->val = concat(node->inh, node->child->val);
        }
        else
        {
            // M → 0 M or M → 1 M or M → D M
            if (parent->type == 'N')
            {
                node->inh = strdup(parent->child->val);
            }
            else
            {
                node->inh = concat(parent->inh, parent->child->val);
            }
            Treenode *firstChild = node->child;
            Treenode *secondChild = firstChild->sibling;
            setatt(firstChild, node);
            setatt(secondChild, node);
            node->val = strdup(secondChild->val); // Copy value from the sibling
        }

        break;
    case 'X':
    case '+':
    case '-':
    case '^':
    case 'x':
        // For other nodes (like individual digits), no action needed
        break;
    default:
        node->val = malloc(2 * sizeof(char)); // Allocate space for one char and the null terminator
        if (node->val)
        {
            node->val[0] = node->type; // Set the first character
            node->val[1] = '\0';       // Null-terminate the string
        }
    }

    // Now, recursively set attributes for all children
    Treenode *child = node->child;
    while (child != NULL)
    {
        setatt(child, node);
        child = child->sibling;
    }
}
// Helper function to extract the power of X from a node
int extractPower(Treenode *node)
{
    int defaultValue = 1;
    if (node->child->sibling != NULL)
    {
        defaultValue = atoi(node->child->sibling->sibling->val);
        // printf("X->x^%d\n",defaultValue);
    }

    return defaultValue;
}
// Helper function to evaluate the polynomial
void helperEvalPoly(Treenode *node, int n, int *sum)
{
    if (node == NULL)
        return;
    if (node->type == 'T')
    {
        if ((node->child->sibling == NULL) && (node->child->type != 'X'))
        {
            // printf("T->1|N\n");
            if (strcmp(node->inh, "-") == 0)
            {
                *sum -= atoi(node->child->val);
            }
            else
            {
                *sum += atoi(node->child->val);
            }
        }
        else if (node->child->type == 'X')
        {
            // printf("T->X\n");
            if (strcmp(node->inh, "-") == 0)
            {
                *sum -= binExp(n, extractPower(node->child));
            }
            else
            {
                *sum += binExp(n, extractPower(node->child));
            }
        }
        else
        {
            // printf("T->NX\n");
            int N = atoi(node->child->val);
            if (strcmp(node->inh, "-") == 0)
            {
                *sum -= N * binExp(n, extractPower(node->child->sibling));
            }
            else
            {
                *sum += N * binExp(n, extractPower(node->child->sibling));
            }
        }
    }
    else
    {
        Treenode *sibling = node->child;
        while (sibling != NULL)
        {
            helperEvalPoly(sibling, n, sum); // Call evalpoly on each child/sibling
            sibling = sibling->sibling;      // Move to the next sibling
        }
    }
}
// Function to evaluate the polynomial for a given value of x
void evalpoly(Treenode *node, int x)
{
    int sum = 0;
    helperEvalPoly(node, x, &sum);
    printf("Value of f(%d) = %d\n", x, sum);
}
// Helper function to print the derivative of the polynomial
void helperPrintDerivative(Treenode *node, int isFirst, char **output)
{
    if (node == NULL)
        return;

    if (node->type == 'S')
    {
        Treenode *sibling = node->child;
        while (sibling != NULL)
        {
            helperPrintDerivative(sibling, isFirst, output);
            sibling = sibling->sibling;
            isFirst = 0;
        }
    }
    else if (node->type == 'T')
    {
        if (node->child->type == 'X')
        {
            int powerOfX = extractPower(node->child);
            if (powerOfX == 1)
            {
                // X->x
                if (strcmp(node->inh, "-") == 0)
                {
                    *output = concat(*output, "-1");
                }
                else
                {
                    *output = concat(*output, (isFirst == 0) ? "+1" : "1");
                }
            }
            else
            {
                // X->x^N
                if (strcmp(node->inh, "-") == 0)
                {
                    *output = concat(*output, "-");
                    *output = concat(*output, powerOfX == 1 ? "" : intToStr(powerOfX));
                    *output = concat(*output, "x");
                    *output = concat(*output,powerOfX==2?"":"^");
                    *output = concat(*output, powerOfX - 1 == 1 ? "" : intToStr(powerOfX - 1));
                }
                else
                {
                    *output = concat(*output, (isFirst == 0) ? "+" : "");
                    *output = concat(*output, powerOfX == 1 ? "" : intToStr(powerOfX));
                    *output = concat(*output, "x");
                    *output = concat(*output,powerOfX==2?"":"^");
                    *output = concat(*output, powerOfX - 1 == 1 ? "" : intToStr(powerOfX - 1));
                }
            }
        }

        if (node->child->sibling != NULL && node->child->sibling->type == 'X')
        {
            int powerOfX = extractPower(node->child->sibling);
            int N = atoi(node->child->val);
            if (powerOfX == 1)
            {
                if (strcmp(node->inh, "-") == 0)
                {
                    *output = concat(*output, "-");
                }
                else
                {
                    *output = concat(*output, (isFirst == 0) ? "+" : "");
                    *output = concat(*output, N == 1 ? "" : intToStr(N));
                }
            }
            else
            {
                if (strcmp(node->inh, "-") == 0)
                {
                    *output = concat(*output, "-");
                    *output = concat(*output, N * powerOfX == 1 ? "" : intToStr(N * powerOfX));
                    *output = concat(*output, "x");
                    *output = concat(*output,powerOfX==2?"":"^");
                    *output = concat(*output, powerOfX - 1 == 1 ? "" : intToStr(powerOfX - 1));
                }
                else
                {
                    *output = concat(*output, (isFirst == 0) ? "+" : "");
                    *output = concat(*output, N * powerOfX == 1 ? "" : intToStr(N * powerOfX));
                    *output = concat(*output, "x");
                    *output = concat(*output,powerOfX==2?"":"^");
                    *output = concat(*output, powerOfX - 1 == 1 ? "" : intToStr(powerOfX - 1));
                }
            }
        }
        isFirst = 0;
    }
    else
    {
        Treenode *sibling = node->child;
        while (sibling != NULL)
        {
            helperPrintDerivative(sibling, 0, output);
            sibling = sibling->sibling;
        }
    }
}
// Function to print the derivative of the polynomial
void printderivative(Treenode *node, int isFirst)
{
    char *output = strdup("");   // Initialize output
    helperPrintDerivative(node, isFirst, &output); // Pass output as a reference

    // Check if output is empty
    if (strcmp(output, "") == 0)
    {
        printf("Derivative: 0\n");
        free(output); // Free the allocated memory
        return;
    }

    // Skip leading '+', if present
    char *result = output; // Store the original pointer to free later
    if (output[0] == '+')
    {
        result = output + 1; // Skip the leading '+'
    }

    printf("Derivative: %s\n", result);
    free(output); // Free the allocated memory
}

int main()
{
    yyparse();
    setatt(headOfProduction,NULL);
    printTree(headOfProduction,0,1);
    printf("\n");
    for(int i=-5;i<=5;i++){
        evalpoly(headOfProduction,i);
    }
    printf("\n");
    printderivative(headOfProduction,1);

    return 0;
}