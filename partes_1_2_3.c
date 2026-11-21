
#include <stdio.h>

int F(char palavra[], int *pos);
int B(char palavra[], int *pos);
int E(char palavra[], int *pos);
int A(char palavra[], int *pos);
int T(char palavra[], int *pos);

/* E -> T A */
int E(char palavra[], int *pos) {

    if (T(palavra, pos) &&
        A(palavra, pos))
        return 1;
    else
        return 0;
}

/*
 * A -> + E A
 * A -> - E A
 * A -> e
 */
int A(char palavra[], int *pos) {

    if (palavra[*pos] == '+') {

        if (match('+', palavra, pos) &&
            E(palavra, pos) &&
            A(palavra, pos))
            return 1;
        else
            return 0;
    }

    else if (palavra[*pos] == '-') {

        if (match('-', palavra, pos) &&
            E(palavra, pos) &&
            A(palavra, pos))
            return 1;
        else
            return 0;
    }

    else {
        return 1;
    }
}

/* T -> F B */
int T(char palavra[], int *pos) {

    if (F(palavra, pos) &&
        B(palavra, pos))
        return 1;
    else
        return 0;
}


int E(char palavra[], int *pos);
int A(char palavra[], int *pos);
int T(char palavra[], int *pos);
int B(char palavra[], int *pos);
int F(char palavra[], int *pos);
int N(char palavra[], int *pos);
int X(char palavra[], int *pos);

/* conta quantos '(' ainda nao foram fechados; serve para saber
   se um ')' encontrado e legitimo ou esta sobrando */
static int abertos = 0;

/* auxiliar: diz se o caractere e um digito de 0 a 9 */
static int eh_digito(char c)
{
    return c >= '0' && c <= '9';
}

/* auxiliar: imprime a mensagem de erro com um '^' apontando a posicao
   e devolve 0, para ser usada: return erro_sintatico(...); */
static int erro_sintatico(char palavra[], int pos, const char *msg)
{
    int i;

    printf("\nERRO sintatico na posicao %d: %s\n", pos + 1, msg);
    printf("  %s\n  ", palavra);
    for (i = 0; i < pos; i++)
        printf(" ");
    printf("^\n");
    return 0;
}

/* auxiliar: erro quando, depois de um operando completo, aparece um
   simbolo que nao pode estar ali (usado quando A ou B derivam eps) */
static int erro_apos_operando(char palavra[], int pos)
{
    char msg[100];
    char c = palavra[pos];

    if (c == '(') {
        return erro_sintatico(palavra, pos,
            "falta operador antes de '(' (ex.: 2*(3+1))");
    } else if (c == ')') {
        return erro_sintatico(palavra, pos,
            "')' sem '(' correspondente");
    } else {
        sprintf(msg, "caractere invalido '%c': esperado operador (+ - * /), ')' ou fim da expressao", c);
        return erro_sintatico(palavra, pos, msg);
    }
}

/* E -> T A
   uma expressao e um termo seguido de zero ou mais somas/subtracoes */
int E(char palavra[], int *pos)
{
    if (T(palavra, pos) &&
        A(palavra, pos))
        return 1;
    else
        return 0;
}

/*
 * A -> + E A
 * A -> - E A
 * A -> eps
 */
int A(char palavra[], int *pos)
{
    char c = palavra[*pos];

    if (c == '+') {
        if (match('+', palavra, pos) &&
            E(palavra, pos) &&
            A(palavra, pos))
            return 1;
        else
            return 0;
    }

    else if (c == '-') {
        if (match('-', palavra, pos) &&
            E(palavra, pos) &&
            A(palavra, pos))
            return 1;
        else
            return 0;
    }

    else {
        /* regra A -> eps: so vale se o proximo simbolo pode vir depois
           de A, ou seja, fim da expressao ou ')' de um parenteses aberto */
        if (c == '\0')
            return 1;
        if (c == ')' && abertos > 0)
            return 1;
        /* qualquer outra coisa e erro */
        return erro_apos_operando(palavra, *pos);
    }
}

/* T -> F B
   um termo e um fator seguido de zero ou mais multiplicacoes/divisoes */
int T(char palavra[], int *pos)
{
    if (F(palavra, pos) &&
        B(palavra, pos))
        return 1;
    else
        return 0;
}

/*
 * B -> * T B
 * B -> / T B
 * B -> eps
 */
int B(char palavra[], int *pos)
{
    char c = palavra[*pos];

    if (c == '*') {
        if (match('*', palavra, pos) &&
            T(palavra, pos) &&
            B(palavra, pos))
            return 1;
        else
            return 0;
    }

    else if (c == '/') {
        if (match('/', palavra, pos) &&
            T(palavra, pos) &&
            B(palavra, pos))
            return 1;
        else
            return 0;
    }

    else {
        /* regra B -> eps: o proximo simbolo pode ser fim, + , - ou ')'
           (os simbolos que podem vir depois de B) */
        if (c == '\0' || c == '+' || c == '-')
            return 1;
        if (c == ')' && abertos > 0)
            return 1;
        return erro_apos_operando(palavra, *pos);
    }
}

/*
 * F -> (E)
 * F -> -N
 * F -> N
 */
int F(char palavra[], int *pos)
{
    char c = palavra[*pos];
    char msg[100];
    int ok;

    if (c == '(') {
        /* regra F -> (E) */
        match('(', palavra, pos);

        /* "()" nao tem expressao dentro */
        if (palavra[*pos] == ')')
            return erro_sintatico(palavra, *pos,
                "parenteses vazios: esperada uma expressao dentro de '( )'");

        /* abertos fica incrementado so enquanto E roda, para A e B
           saberem que um ')' aqui fecha este parenteses */
        abertos++;
        ok = E(palavra, pos);
        abertos--;
        if (!ok)
            return 0;

        /* depois da expressao interna tem que vir ')' */
        if (palavra[*pos] != ')') {
            if (palavra[*pos] == '\0')
                return erro_sintatico(palavra, *pos,
                    "falta fechar parenteses: esperado ')'");
            else
                return erro_sintatico(palavra, *pos, "esperado ')'");
        }
        return match(')', palavra, pos);
    }

    else if (c == '-') {
        /* regra F -> -N: o sinal so vale diante de um numero */
        match('-', palavra, pos);
        if (!eh_digito(palavra[*pos]))
            return erro_sintatico(palavra, *pos,
                "apos '-' esperado um numero (sinal so vale diante de digitos)");
        return N(palavra, pos);
    }

    else if (eh_digito(c)) {
        /* regra F -> N */
        return N(palavra, pos);
    }

    /* abaixo: nenhuma regra serve, entao e erro; a mensagem
       depende do que foi encontrado no lugar do operando */
    else if (c == '\0') {
        return erro_sintatico(palavra, *pos,
            "expressao incompleta: esperado numero, '(' ou '-'");
    }

    else if (c == '+' || c == '*' || c == '/') {
        sprintf(msg, "operador '%c' inesperado: falta um operando antes dele", c);
        return erro_sintatico(palavra, *pos, msg);
    }

    else if (c == ')') {
        return erro_sintatico(palavra, *pos,
            "')' inesperado: falta um operando antes dele");
    }

    else {
        sprintf(msg, "caractere invalido '%c': esperado numero, '(' ou '-'", c);
        return erro_sintatico(palavra, *pos, msg);
    }
}

/*
 * X -> N
 * X -> eps
 * depois do primeiro digito, continua lendo digitos enquanto houver
 */
int X(char palavra[], int *pos)
{
    if (eh_digito(palavra[*pos]))
        return N(palavra, pos);

    /* eps: nao consome nada; o simbolo seguinte sera
       conferido por A ou B */
    return 1;
}