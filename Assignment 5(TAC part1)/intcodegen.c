#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "intcodegen.h"

#define MAX_VARS 1000
#define MAX_TEMPS 10
#define BUFFER_SIZE 50 // For formatting strings

int next_mem = 0;
int next_temp = 2;
// int next_mem_temp = 0;

struct
{
    char *name;
    int offset;
} symbol_table[MAX_VARS];

int symbol_count = 0;

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

// Helper function to return formatted string based on type (MEM, R, or number)
char *format_string(int type, int value)
{
    char *result = (char *)malloc(BUFFER_SIZE);
    if (type == 0)
    {
        snprintf(result, BUFFER_SIZE, "0/%d", value); // Memory (MEM)
    }
    else if (type == 1)
    {
        snprintf(result, BUFFER_SIZE, "1/%d", value); // Register (R)
    }
    else if (type == 2)
    {
        snprintf(result, BUFFER_SIZE, "2/%d", value); // Direct number
    }
    return result;
}

void init_code_gen()
{
    printf("#include <stdio.h>\n");
    printf("#include <stdlib.h>\n");
    printf("#include \"aux.c\"\n");
    printf("\nint main ( ) \n{\n");
    printf("    int R[12];\n");
    printf("    int MEM[65536];\n\n");
}

void finalize_code_gen()
{
    printf("    exit(0);\n");
    printf("}\n");
}

int get_var_offset(char *id)
{
    for (int i = 0; i < symbol_count; i++)
    {
        if (strcmp(symbol_table[i].name, id) == 0)
        {
            return symbol_table[i].offset;
        }
    }
    symbol_table[symbol_count].name = strdup(id);
    symbol_table[symbol_count].offset = next_mem++;
    symbol_count++;
    return next_mem - 1;
}


// Handle set with a number
char *generate_set_num(char *id, char *num)
{
    int offset = get_var_offset(id);
    printf("    MEM[%d] = %s;\n", offset, num);
    printf("    mprn(MEM,%d);\n", offset);
    return format_string(0, offset); // Return memory address (MEM)
}

// Handle set with a value from another variable
char *generate_set_id(char *id1, char *id2)
{
    int offset1 = get_var_offset(id1);
    int offset2 = get_var_offset(id2);
    printf("    R[0] = MEM[%d];\n", offset2);
    printf("    MEM[%d] = R[0];\n", offset1);
    printf("    mprn(MEM,%d);\n", offset1);
    return format_string(0, offset1); // Return memory address (MEM)
}

// Handle set with an expression (result in register)
char *generate_set_expr(char *id, char *expr_reg)
{
    int offset = get_var_offset(id);
    printf("    MEM[%d] = R[%d];\n", offset, atoi(expr_reg + 2));
    printf("    mprn(MEM,%d);\n", offset);
    next_temp = atoi(expr_reg + 2);
    return format_string(0, offset); // Return memory address (MEM)
}

// Handle standalone expressions
void generate_expr_stmt(char *expr_reg)
{
    printf("    eprn(R,%d);\n", atoi(expr_reg + 2));
    int nextReg = atoi(expr_reg + 2);
    if (nextReg > 2)
        next_temp = nextReg;
}

// Function to check for the next available temporary register (R[2] to R[11])
int get_available_temp_register()
{
    if (next_temp < MAX_TEMPS)
    {
        next_temp++;
        return next_temp - 1;
    }
    else
        return 0;
}
/*
    Format of the string: type/value
    type 0/... means memory ... is address
    type 1/... means register ... is address
    type 2/... means direct number ... is value
*/

// Handle binary operations
char *generate_op(char *op, char *arg1, char *arg2)
{

    if (strcmp(op, "**") == 0)
    {
        if (arg1[0] == '0' && arg2[0] == '0')
        {
            // printf("//MEM MEM\n");
            printf("    R[0]=MEM[%d];\n", atoi(arg1 + 2));
            printf("    R[1]=MEM[%d],\n", atoi(arg2 + 2));
            int newreg = get_available_temp_register();
            if (newreg == 0)
            {
                printf("    R[0]=pwr(R[0],R[1]);\n");
                printf("    MEM[%d]=R[0];\n", next_mem++);
                return format_string(0, next_mem - 1);
            }
            else
            {
                printf("    R[%d]=pwr(R[0],R[1]);\n", newreg);
                return format_string(1, newreg);
            }
        }
        else if (arg1[0] == '0' && arg2[0] == '1')
        {
            // printf("//MEM R\n");
            printf("    R[0]=MEM[%d];\n", atoi(arg1 + 2));
            printf("    R[%d]=pwr(R[0],R[%d]);\n", atoi(arg2 + 2), atoi(arg2 + 2));
            next_temp = atoi(arg2 + 2) + 1;
            return format_string(1, atoi(arg2 + 2));
        }
        else if (arg1[0] == '0' && arg2[0] == '2')
        {
            // printf("//MEM NUM\n");
            printf("    R[0]=MEM[%d];\n", atoi(arg1 + 2));
            int newreg = get_available_temp_register();
            if (newreg == 0)
            {
                printf("    R[0]=pwr(R[0],%d);\n", atoi(arg2 + 2));
                printf("    MEM[%d]=R[0];\n", next_mem++);
                return format_string(0, next_mem - 1);
            }
            else
            {
                printf("    R[%d]=pwr(R[0],%d);\n", newreg, atoi(arg2 + 2));
                return format_string(1, newreg);
            }
        }
        else if (arg1[0] == '1' && arg2[0] == '0')
        {
            // printf("//R MEM\n");
            printf("    R[0]=MEM[%d];\n", atoi(arg2 + 2));
            printf("    R[%d]=pwr(R[%d],R[0]);\n", atoi(arg1 + 2), atoi(arg1 + 2));
            return format_string(1, atoi(arg1 + 2));
        }
        else if (arg1[0] == '1' && arg2[0] == '1')
        {
            // printf("//R R\n");
            printf("    R[%d]=pwr(R[%d],R[%d]);\n", atoi(arg1 + 2), atoi(arg1 + 2), atoi(arg2 + 2));
            return format_string(1, atoi(arg1 + 2));
        }
        else if (arg1[0] == '1' && arg2[0] == '2')
        {
            // printf("//R NUM\n");
            printf("    R[%d]=pwr(R[%d],%d);\n", atoi(arg1 + 2), atoi(arg1 + 2), atoi(arg2 + 2));
            return format_string(1, atoi(arg1 + 2));
        }
        else if (arg1[0] == '2' && arg2[0] == '0')
        {
            // printf("//NUM MEM\n");
            printf("    R[0]=MEM[%d];\n", atoi(arg2 + 2));
            int newreg = get_available_temp_register();
            if (newreg == 0)
            {
                printf("    R[0]=pwr(R[0],%d);\n", atoi(arg1 + 2));
                printf("    MEM[%d]=R[0];\n", next_mem++);
                return format_string(0, next_mem - 1);
            }
            else
            {
                printf("    R[%d]=pwr(R[0],%d);\n", newreg, atoi(arg1 + 2));
                return format_string(1, newreg);
            }
        }
        else if (arg1[0] == '2' && arg2[0] == '1')
        {
            // printf("//NUM R\n");
            printf("    R[%d]=pwr(%d,R[%d]);\n", atoi(arg2 + 2), atoi(arg1 + 2), atoi(arg2 + 2));
            return format_string(1, atoi(arg2 + 2));
        }
        else if (arg1[0] == '2' && arg2[0] == '2')
        {
            // printf("//NUM NUM\n");
            int newreg = get_available_temp_register();
            if (newreg == 0)
            {
                printf("    R[0]=pwr(%d,%d);\n", atoi(arg1 + 2), atoi(arg2 + 2));
                printf("    MEM[%d]=R[0];\n", next_mem++);
                return format_string(0, next_mem - 1);
            }
            else
            {
                printf("    R[%d]=pwr(%d,%d);\n", newreg, atoi(arg1 + 2), atoi(arg2 + 2));
                return format_string(1, newreg);
            }
        }
    }

    if (arg1[0] == '0' && arg2[0] == '0')
    {
        // printf("//MEM MEM\n");
        printf("    R[0]=MEM[%d];\n", atoi(arg1 + 2));
        printf("    R[1]=MEM[%d],\n", atoi(arg2 + 2));
        int newreg = get_available_temp_register();
        if (newreg == 0)
        {
            printf("    R[0]=R[0]%sR[1];\n", op);
            printf("    MEM[%d]=R[0];\n", next_mem++);
            return format_string(0, next_mem - 1);
        }
        else
        {
            printf("    R[%d]=R[0]%sR[1];\n", newreg, op);
            return format_string(1, newreg);
        }
    }
    else if (arg1[0] == '0' && arg2[0] == '1')
    {
        // printf("//MEM R\n");
        printf("    R[0]=MEM[%d];\n", atoi(arg1 + 2));
        printf("    R[%d]=R[0]%sR[%d];\n", atoi(arg2 + 2), op, atoi(arg2 + 2));
        next_temp = atoi(arg2 + 2) + 1;
        return format_string(1, atoi(arg2 + 2));
    }
    else if (arg1[0] == '0' && arg2[0] == '2')
    {
        // printf("//MEM NUM\n");
        printf("    R[0]=MEM[%d];\n", atoi(arg1 + 2));
        int newreg = get_available_temp_register();
        if (newreg == 0)
        {
            printf("    R[0]=R[0]%s%d;\n", op, atoi(arg2 + 2));
            printf("    MEM[%d]=R[0];\n", next_mem++);
            return format_string(0, next_mem - 1);
        }
        else
        {
            printf("    R[%d]=R[0]%s%d;\n", newreg, op, atoi(arg2 + 2));
            return format_string(1, newreg);
        }
    }
    else if (arg1[0] == '1' && arg2[0] == '0')
    {
        // printf("//R MEM\n");
        printf("    R[0]=MEM[%d];\n", atoi(arg2 + 2));
        printf("    R[%d]=R[%d]%sR[0];\n", atoi(arg1 + 2), atoi(arg1 + 2), op);
        return format_string(1, atoi(arg1 + 2));
    }
    else if (arg1[0] == '1' && arg2[0] == '1')
    {
        // printf("//R R\n");
        printf("    R[%d]=R[%d]%sR[%d];\n", atoi(arg1 + 2), atoi(arg1 + 2), op, atoi(arg2 + 2));
        return format_string(1, atoi(arg1 + 2));
    }
    else if (arg1[0] == '1' && arg2[0] == '2')
    {
        // printf("//R NUM\n");
        printf("    R[%d]=R[%d]%s%d;\n", atoi(arg1 + 2), atoi(arg1 + 2), op, atoi(arg2 + 2));
        return format_string(1, atoi(arg1 + 2));
    }
    else if (arg1[0] == '2' && arg2[0] == '0')
    {
        // printf("//NUM MEM\n");
        printf("    R[0]=MEM[%d];\n", atoi(arg2 + 2));
        int newreg = get_available_temp_register();
        if (newreg == 0)
        {
            printf("    R[0]=R[0]%s%d;\n", op, atoi(arg1 + 2));
            printf("    MEM[%d]=R[0];\n", next_mem++);
            return format_string(0, next_mem - 1);
        }
        else
        {
            printf("    R[%d]=R[0]%s%d;\n", newreg, op, atoi(arg1 + 2));
            return format_string(1, newreg);
        }
    }
    else if (arg1[0] == '2' && arg2[0] == '1')
    {
        // printf("//NUM R\n");
        // printf("//%s %s %s\n", arg1, op, arg2);
        printf("    R[%d]=%d%sR[%d];\n", atoi(arg2 + 2), atoi(arg1 + 2), op, atoi(arg2 + 2));
        return format_string(1, atoi(arg2 + 2));
    }
    else if (arg1[0] == '2' && arg2[0] == '2')
    {
        // printf("//NUM NUM\n");
        int newreg = get_available_temp_register();
        if (newreg == 0)
        {
            printf("    R[0]=%d%s%d;\n", atoi(arg1 + 2), op, atoi(arg2 + 2));
            printf("    MEM[%d]=R[0];\n", next_mem++);
            return format_string(0, next_mem - 1);
        }
        else
        {
            printf("    R[%d]=%d%s%d;\n", newreg, atoi(arg1 + 2), op, atoi(arg2 + 2));
            return format_string(1, newreg);
        }
    }

    // Return default if none of the conditions are met
    printf("//Unmatched case for binary operation\n");
    return NULL; // Default return to avoid warning
}

int main()
{
    init_code_gen();
    yyparse(); // Assuming yyparse is called to parse and generate the code
    finalize_code_gen();
    return 0;
}
