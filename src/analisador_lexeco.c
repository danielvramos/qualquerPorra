
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define MAX_TOKENS 500
#define MAX_TOKEN_LENGTH 100
#define MAX_CHILDREN 10

/******************************************************************************

                        DECLARAÇÕES

*******************************************************************************/




// Enumeração dos tipos de tokens
typedef enum {
    PALAVRA_CHAVE, OPERADOR_LOGICO, OPERADOR_RELACIONAL, OPERADOR_ARITMETICO,
    ATRIBUICAO, DELIMITADOR, NUMERO_INTEIRO, NUMERO_REAL, IDENTIFICADOR,
    TOKEN_DESCONHECIDO
} TipoToken;

// Estrutura que representa um token
typedef struct {
    TipoToken tipo;
    char lexema[MAX_TOKEN_LENGTH];
    int linha;  
} Token;

// Contadores para o resumo de tokens
int contador_palavras_chave = 0;
int contador_identificadores = 0;
int contador_inteiros = 0;
int contador_reais = 0;
int contador_operadores = 0;
int contador_delimitadores = 0;
int contador_token_desconhecido = 0;

// Palavras-chave da linguagem P-
const char* palavras_chave[] = {
    "inteiro", "real", "se", "entao", "senao", "enquanto", "repita", "ate", "ler", "mostrar"
};
int num_palavras_chave = 10;

typedef struct node 
{ 
    char* info;  
    int type;
    int childCount;  
    struct node* children[MAX_CHILDREN];  
    
}Node;


/******************************************************************************

                        METODOS E FUNÇÕES DO ANALISADOR LÉXICO

*******************************************************************************/



// Função que verifica se o lexema é uma palavra-chave
int eh_palavra_chave(const char* lexema) {
    for (int i = 0; i < num_palavras_chave; i++) {
        if (strcmp(lexema, palavras_chave[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Função que classifica o token com base no lexema
Token classificar_token(const char* lexema, int linha) {
    Token token;
    strcpy(token.lexema, lexema);
    token.linha = linha;
    if (eh_palavra_chave(lexema)) {
        token.tipo = PALAVRA_CHAVE;
        contador_palavras_chave++;
     

    } else if (strcmp(lexema, "&&") == 0 || strcmp(lexema, "||") == 0) {
        token.tipo = OPERADOR_LOGICO;
        contador_operadores++;
    } else if (strcmp(lexema, "<") == 0 || strcmp(lexema, "<=") == 0 ||
               strcmp(lexema, ">") == 0 || strcmp(lexema, ">=") == 0 ||
               strcmp(lexema, "==") == 0 || strcmp(lexema, "!=") == 0) {
        token.tipo = OPERADOR_RELACIONAL;
        contador_operadores++;
    } else if (strcmp(lexema, "+") == 0 || strcmp(lexema, "-") == 0 ||
               strcmp(lexema, "*") == 0 || strcmp(lexema, "/") == 0) {
        token.tipo = OPERADOR_ARITMETICO;
        contador_operadores++;
    } else if (strcmp(lexema, "=") == 0) {
        token.tipo = ATRIBUICAO;
        contador_operadores++;
    } else if (strcmp(lexema, ";") == 0 || strcmp(lexema, ",") == 0 ||
               strcmp(lexema, "(") == 0 || strcmp(lexema, ")") == 0 ||
               strcmp(lexema, "{") == 0 || strcmp(lexema, "}") == 0) {
        token.tipo = DELIMITADOR;
        contador_delimitadores++;
    } else if (isdigit(lexema[0])) {
        if (strchr(lexema, '.') != NULL) {
            token.tipo = NUMERO_REAL;
            contador_reais++;
        } else {
            token.tipo = NUMERO_INTEIRO;
            contador_inteiros++;
        }
    } else if (isalpha(lexema[0]) || lexema[0] == '_') {
        token.tipo = IDENTIFICADOR;
        contador_identificadores++;
    } else {
        token.tipo = TOKEN_DESCONHECIDO;
        fprintf(stderr, "Erro lexico: Token desconhecido '%s' encontrado na linha %d. ",
                        lexema, linha);
        exit(1);
    }

    return token;
}

// Função que exibe o token e o seu tipo na formatação desejada
void exibir_token(Token token) {
    const char* tipo_token[] = {
        "PALAVRA-CHAVE", "OPERADOR_LOGICO", "OPERADOR_RELACIONAL", "OPERADOR_ARITMETICO",
        "ATRIBUICAO", "DELIMITADOR", "INTEIRO", "REAL", "IDENTIFICADOR", "TOKEN DESCONHECIDO"
    };

    printf("%-15s : %s\n", tipo_token[token.tipo], token.lexema);
}

void inserirToken(Token *ListaTokens, Token novoToken){
        int i = 0;  
    while (ListaTokens[i].tipo != NULL)  
        i++;
    ListaTokens[i] = novoToken;      
}


// Função que analisa o código de entrada e identifica os tokens
Token * analisar_lexico(const char* codigo, int linha) {
    int i = 0;
    char lexema[MAX_TOKEN_LENGTH];
    int pos_lexema = 0;
    Token ListaTokens[MAX_TOKENS];
    
    while (codigo[i] != '\0') {
        // Ignora espaços em branco
        if (isspace(codigo[i])) {
            i++;
            continue;
        }

        // Identifica identificadores ou palavras-chave
        if (isalpha(codigo[i]) || codigo[i] == '_') {
            pos_lexema = 0;
            while (isalnum(codigo[i]) || codigo[i] == '_') {
                lexema[pos_lexema++] = codigo[i++];
            }
            lexema[pos_lexema] = '\0';
            Token token = classificar_token(lexema, linha);
            inserirToken(ListaTokens, token);
            exibir_token(token);
        }
        // Identifica números inteiros ou reais
        else if (isdigit(codigo[i])) {
            pos_lexema = 0;
            while (isdigit(codigo[i])) {
                lexema[pos_lexema++] = codigo[i++];
            }
            if (codigo[i] == '.') {
                lexema[pos_lexema++] = codigo[i++];
                while (isdigit(codigo[i])) {
                    lexema[pos_lexema++] = codigo[i++];
                }
            }
            lexema[pos_lexema] = '\0';
            Token token = classificar_token(lexema, linha);
            inserirToken(ListaTokens, token);
            exibir_token(token);
        }
        // Identifica operadores, delimitadores e símbolos
        else {
            lexema[0] = codigo[i++];
            lexema[1] = '\0';

            // Verifica operadores lógicos
            if (lexema[0] == '&' && codigo[i] == '&') {
                lexema[1] = codigo[i++];
                lexema[2] = '\0';
            } else if (lexema[0] == '|' && codigo[i] == '|') {
                lexema[1] = codigo[i++];
                lexema[2] = '\0';
            }
            // Verifica operadores relacionais de dois caracteres (==, !=, <=, >=)
            else if (lexema[0] == '=' || lexema[0] == '!' ||
                     lexema[0] == '<' || lexema[0] == '>') {
                if (codigo[i] == '=') {
                    lexema[1] = codigo[i++];
                    lexema[2] = '\0';
                }
            }

            Token token = classificar_token(lexema, linha);
            inserirToken(ListaTokens, token);
            exibir_token(token);
        }
    }
    return ListaTokens;
}

// Função para exibir o resumo dos tokens
void exibir_resumo_tokens() {
    printf("\nResumo de Tokens:\n");
    printf("Palavras-chave    : %d\n", contador_palavras_chave);
    printf("Identificadores   : %d\n", contador_identificadores);
    printf("Números Inteiros  : %d\n", contador_inteiros);
    printf("Números Reais     : %d\n", contador_reais);
    printf("Operadores        : %d\n", contador_operadores);
    printf("Delimitadores     : %d\n", contador_delimitadores);
    printf("Tokens Desconhecidos : %d\n", contador_token_desconhecido);
}

/******************************************************************************

                        FUNC E METODOS DO ANALISADOR SINTÁTICO

*******************************************************************************/

Node* CreateNode(char *value, int ntype)  
{
    Node* p = (Node*)malloc(sizeof(Node));  
    int i;  
    p->info = value;  
    p->type = ntype;  
    for (i = 0; i < MAX_CHILDREN; i++)  
        p->children[i] = NULL;  return p;  
    
}
Node* AddChild(Node* node, char *value, int ntype)  
{
    int i = 0;  
    while (node->children[i] != NULL)  
        i++;  
    if (i < MAX_CHILDREN)  
        node->children[i] = CreateNode(value, ntype);  
    else  
        printf("Error: MAX_CHILDREN!\n");  
    return node->children[i];  }


void PrintTree(Node *node, int tabs)  
{
    int i = 0;  
    int x = 0;  
    for (x = 0; x < tabs; x++)  
        printf("\t");  printf("%s\n", node->info);  
    while (node->children[i] != NULL)  
    {
        PrintTree(node->children[i], tabs + 1);
        i++;  
        
    }
    
}


Node* buildDeclarationTree(char** tokens, int tokenCount) {
    Node* root = createNode(tokens[0], "TYPE"); // Assume que o primeiro token é o tipo
    Node* current = root;
    int i = 1;

    while (i < tokenCount) {
        if (strcmp(tokens[i], "=") == 0) {
            // Encontrou uma atribuição
            Node* assignment = AddChild(current,"=", "OPERATOR");
            current = assignment;
            i++; // Avança para a expressão
        } else if (strcmp(tokens[i], ";") == 0) {
            // Fim da declaração
            break;
        } else {
            // Adiciona um novo filho (identificador ou expressão)
            Node* newNode = createNode(tokens[i], "IDENTIFIER"); // Assume que o restante são identificadores
            current->children[current->childCount++] = newNode;
        }
        i++;
    }

    return root;
}

/*
int termo(FILE *code_file, int next_token, NextChar *next_char,  Node *tree)
{
    printf("Enter <termo>\n");  
    Node *termonode = AddChild(tree, "termo", -1); 
    next_token = fator(code_file, next_token, next_char, termonode);
    while (next_token == MULT_OP || next_token == DIV_OP)  
    {
        if (next_token == MULT_OP)  
            AddChild(termonode, "*", next_token);  
        else 
            if (next_token == DIV_OP)  
            AddChild(termonode, "/", next_token);
        next_token = lex(code_file, next_char);
        next_token = fator(code_file, next_token, next_char, termonode);  
        
    }
    printf("Exit <termo>\n");
    return next_token;  

}*/

/******************************************************************************

                      FUNÇÃO MAIN

*******************************************************************************/

// Função principal que recebe o caminho do arquivo e chama o analisador léxico
int main(int argc, char *argv[]) {
    // Verifica se o número de argumentos está correto
    if (argc != 3) {
        printf("Uso: %s -p <caminho_do_arquivo>\n", argv[0]);
        return 1;
    }

    // Verifica se o argumento -p foi passado
    if (strcmp(argv[1], "-p") == 0) {
        const char *filepath = argv[2];  // Caminho do arquivo passado pelo usuário
        printf("Caminho do arquivo recebido: %s\n", filepath);

        FILE *arquivo = fopen(filepath, "r");
        if (arquivo == NULL) {
            perror("Erro ao abrir o arquivo");
            return 1;
        }

        // Lê o conteúdo do arquivo linha por linha
        char linha[256];
        int linha_atual = 1;
        printf("Lendo o arquivo %s...\n\n", filepath);
        while (fgets(linha, sizeof(linha), arquivo)) {
            Token *ListaTokens;

            ListaTokens = analisar_lexico(linha, linha_atual);
            linha_atual++;
        }

        // Fecha o arquivo após a leitura
        fclose(arquivo);

        // Exibe o resumo dos tokens
        exibir_resumo_tokens();
    } else {
        printf("Argumento inválido! Uso: %s -p <caminho_do_arquivo>\n", argv[0]);
        return 1;
    }

    return 0;
}
