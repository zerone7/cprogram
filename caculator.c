/**********************************************************************************
 * 中缀表达式转后缀表达式
 *     从左至右依次扫描中缀表达式每个字符，做如下处理:
 *     1) 若为数字，则直接输出;
 *     2) 若为运算符，则如下处理：
 *        a) 运算符为'('，直接入栈。
 *        b) 运算符为')'，依次把栈中的运算符输出，直到遇到'('，并从栈中删除'('
 *        c) 运算符为(+/-*)：
 *            i)  若运算符的优先级小于或等于栈顶元素的优先级，则取出栈顶元素输出，
 *                并弹出栈顶元素，反复执行直到栈顶元素的优先级小于当前运算符的优
 *                先级。
 *            ii) 若当运算符的优先级大于栈顶元素的优先级时，则将其压入栈。
 *     3) 重复执行以上步骤直到中缀表达式处理完，最后将栈中剩余的所有元素依次弹出。
 *********************************************************************************/
#include <stdio.h>
#include <string.h>

#include "stack.h"

static struct stack *pstack;

static inline int prio(char operator)
{
    switch (operator) {
    case '+':
    case '-':
        return 1;
    case '*':
    case '/':
        return 2;
    case '(':
        return 0;
    default:
        return -1;
    }
}

void infix_to_suffix(char *instr, char *outstr)
{
    int len, i, j = 0;
    char a;

    assert(instr && outstr);
    len = strlen(instr);
    for (i = 0; i < len; i++) {
        a = instr[i];
        if ((a >= '0' && a <= '9') || (a >= 'a' && a <= 'z')) {
            while ((a >= '0' && a <= '9') || (a >= 'a' && a <= 'z')) {
                outstr[j] = a;
                j++; 
                i++;
                a = instr[i];
            }
            i--;
            outstr[j] = ' '; //just add blanck char
            j++;
        } else if (a == '(') {
            push(pstack, a);
        } else if (a == ')') {
            while (top(pstack) != '(') {
                if (top(pstack) == '\0')
                    return;
                else {
                    outstr[j] = pop(pstack);
                    j++;
                    outstr[j] = ' '; //just add blanck char
                    j++;
                }
            }
            pop(pstack);
        } else if (a == '+' || a == '-' || a == '*' || a == '/') {
            while (top(pstack) != '\0' && prio(top(pstack)) >= prio(a)) {
                outstr[j] = pop(pstack);
                j++;
                outstr[j] = ' '; //just add blanck char
                j++;
            }
            push(pstack, a);
        }
    }

    while (!is_empty(pstack)) {
        outstr[j] = pop(pstack);
        j++;
        outstr[j] = ' '; //just add blanck char
        j++;
    }

    outstr[j-1] = '\0';
}

void compute(char *outstr)
{
    int num_stack[100], result, num, i = -1;
    char *tmp;

    tmp = strtok(outstr, " ");
    while (tmp) {
        if (tmp[0] >= '0' && tmp[0] <= '9') {
            i++;
            num = atoi(tmp);
            num_stack[i] = num;
        } else {
            switch (tmp[0]) {
            case '+':
                result = num_stack[i-1] + num_stack[i];
                num_stack[i-1] = result;
                i--;
                break;
            case '-':
                result = num_stack[i-1] - num_stack[i];
                num_stack[i-1] = result;
                i--;
                break;
            case '*':
                result = num_stack[i-1] * num_stack[i];
                num_stack[i-1] = result;
                i--;
                break;
            case '/':
                result = num_stack[i-1] / num_stack[i];
                num_stack[i-1] = result;
                i--;
                break;
            default:
                printf("Invalid token\n");
                break;
            }
        }

        tmp = strtok(NULL, " ");
    }

    printf("result is %d\n", num_stack[0]);
}

int main()
{
    char instr[100], outstr[200];

    pstack = init_stack();
    if (!pstack) {
        perror("init stack faild");
        return -1;
    }

    while (fgets(instr, 100, stdin)) {
        instr[strlen(instr)-1] = '\0';
        infix_to_suffix(instr, outstr);
        printf("%s\n", outstr);
        compute(outstr);
    }

    destroy_stack(pstack);

    return 0;
}
