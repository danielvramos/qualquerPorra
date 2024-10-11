
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
typedef struct Token{
    TipoToken tipo;
    char lexema[MAX_TOKEN_LENGTH];
    int linha;
    struct Token* next; 
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

typedef struct Node {
    Token token;            // Token associado ao n�
    struct Node** children; // Filhos do n� (array din�mico)
    int child_count;        // Contagem de filhos
} Node;
/*
typedef struct node 
{ 
    char* info;  
    int type;
    int childCount;  
    struct node* children[MAX_CHILDREN];  
    
}Node;


*****************************************************************************

                        METODOS E FUNÇÕES DO ANALISADOR LÉXICO

*******************************************************************************/

// Função para criar um novo token e inserir no final da lista
void inserirToken(Token** head, Token *novoToken) {
 

    // Se a lista estiver vazia, o novo token será o primeiro (head)
    if (*head == NULL) {
        *head = novoToken;
    } else {
        // Caso contrário, percorre a lista até o final
        Token* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        // Insere o novo token no final
        temp->next = novoToken;
    }
}

// Função que verifica se o lexema é uma palavra-chave
int eh_palavra_chave(const char* lexema) {
    for (int i = 0; i < num_palavras_chave; i++) {
        if (strcmp(lexema, palavras_chave[i]) == 0) {
            return 1;
        }
    }
    return 0;
}
Token* criarToken(const char* lexema, int linha) {
    Token* novo = (Token*)malloc(sizeof(Token)); // Alocando memória para um novo token
    if (novo == NULL) {
        fprintf(stderr, "Erro ao alocar memória para novo token.\n");
        exit(EXIT_FAILURE);
    }
     // Verifica o comprimento do lexema antes de copiar
    if (strlen(lexema) < MAX_TOKEN_LENGTH) {
        strcpy(novo->lexema, lexema);
    } else {
        fprintf(stderr, "Erro: lexema é muito longo.\n");
        free(novo); // Libera a memória alocada
        exit(EXIT_FAILURE);
    } // Copia o lexema para o novo token

    novo->next = NULL; // Inicializa o próximo ponteiro como NULL
    return novo;
}
// Função que classifica o token com base no lexema
Token* classificar_token(const char* lexema, int linha) {
    Token* token = criarToken(lexema,linha);
    
    
    if (eh_palavra_chave(lexema)) {
        token->tipo = PALAVRA_CHAVE;
        contador_palavras_chave++;
     

    } else if (strcmp(lexema, "&&") == 0 || strcmp(lexema, "||") == 0) {
        token->tipo = OPERADOR_LOGICO;
        contador_operadores++;
    } else if (strcmp(lexema, "<") == 0 || strcmp(lexema, "<=") == 0 ||
               strcmp(lexema, ">") == 0 || strcmp(lexema, ">=") == 0 ||
               strcmp(lexema, "==") == 0 || strcmp(lexema, "!=") == 0) {
        token->tipo = OPERADOR_RELACIONAL;
        contador_operadores++;
    } else if (strcmp(lexema, "+") == 0 || strcmp(lexema, "-") == 0 ||
               strcmp(lexema, "*") == 0 || strcmp(lexema, "/") == 0) {
        token->tipo = OPERADOR_ARITMETICO;
        contador_operadores++;
    } else if (strcmp(lexema, "=") == 0) {
        token->tipo = ATRIBUICAO;
        contador_operadores++;
    } else if (strcmp(lexema, ";") == 0 || strcmp(lexema, ",") == 0 ||
               strcmp(lexema, "(") == 0 || strcmp(lexema, ")") == 0 ||
               strcmp(lexema, "{") == 0 || strcmp(lexema, "}") == 0) {
        token->tipo = DELIMITADOR;
        contador_delimitadores++;
    } else if (isdigit(lexema[0])) {
        if (strchr(lexema, '.') != NULL) {
            token->tipo = NUMERO_REAL;
            contador_reais++;
        } else {
            token->tipo = NUMERO_INTEIRO;
            contador_inteiros++;
        }
    } else if (isalpha(lexema[0]) || lexema[0] == '_') {
        token->tipo = IDENTIFICADOR;
        contador_identificadores++;
    } else {
        token->tipo = TOKEN_DESCONHECIDO;
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




// Método que analisa o código de entrada e identifica os tokens
void analisar_lexico(const char* codigo, int linha,Token** ListaTokens) {
    int i = 0;
    char lexema[MAX_TOKEN_LENGTH];
    int pos_lexema = 0;
    
    
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
            Token* token = classificar_token(lexema, linha);
            inserirToken(ListaTokens, token);
           // exibir_token(*token);
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
            Token* token = classificar_token(lexema, linha);
            inserirToken(ListaTokens, token);
           // exibir_token(token);
          // exibir_token(*token);
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

            Token* token = classificar_token(lexema, linha);
            inserirToken(ListaTokens, token);
         //   exibir_token(*token);
          //  exibir_token(token);
        }
    }

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
/*
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
*/
Node* create_node(Token token) {
    Node* new_node = (Node*)malloc(sizeof(Node)); // Cast necess�rio
    new_node->token = token;
    new_node->children = NULL;
    new_node->child_count = 0;
    return new_node;
}
void add_child(Node* parent, Node* child) {
    parent->children = (Node**)realloc(parent->children, sizeof(Node*) * (parent->child_count + 1)); // Cast necess�rio
    parent->children[parent->child_count] = child;
    parent->child_count++;
}

/*
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
    
}*/
// Função para imprimir a lista de tokens
void imprimirTokens(Token* node) {
    while (node != NULL) {
        printf("Tipo: %d, Lexema: %s, Linha: %d\n", node->tipo, node->lexema, node->linha);
        node = node->next;
    }
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
// Enumeração dos tipos de tokens
typedef enum {
    PALAVRA_CHAVE, OPERADOR_LOGICO, OPERADOR_RELACIONAL, OPERADOR_ARITMETICO,
    ATRIBUICAO, DELIMITADOR, NUMERO_INTEIRO, NUMERO_REAL, IDENTIFICADOR,
    TOKEN_DESCONHECIDO
} TipoToken;
}*/

Token* match(TipoToken expected,Token* token ) {
    if (token->tipo == expected) {
        if (token->next != NULL)
            return token->next;
        else
            return NULL;
    } else {
        printf("\nErro na linha \%d: token '%s'  não esperado.\n",token->lexema);
        exit(1);
    }
}


// <declaration> ::= <type> <IDENTIFICADOR> "=" <valor> ";"
Node* declaration(Token* token) {
    printf("Processando declaracao...\n");
    Node* node = create_node(*token); // N� da declara��o
    printf("Obtendo o tipo...\n");
    token = match(PALAVRA_CHAVE,token);   // tipo (inteiro, real)
    add_child(node, create_node(*token)); // Identificador
    printf("Obtendo o identificado...\n");
    token = match(IDENTIFICADOR,token);   // identificador (x, y)
    printf("Obtendo o identificado...\n");
    token = match(ATRIBUICAO,token);        // operador de atribui��o (=)
    
    // Aceita n�mero inteiro ou real
    if (token->tipo  == NUMERO_INTEIRO) {
        add_child(node, create_node(*token)); // Valor inteiro
        token = match(NUMERO_INTEIRO,token);
    } else if (token->tipo == NUMERO_REAL) {
        add_child(node, create_node(*token)); // Valor real
        token = match(NUMERO_REAL,token);
    } else {
        printf("Erro: valor esperado apos atribuicao.\n");
        exit(1);
    }
    
    token = match(DELIMITADOR,token);     // delimitador (;)
    printf("Declaracao processada com sucesso.\n");
    return node;
}
// <output_statement> ::= "mostrar" <STRING> ";"
Node* output_statement(Token* token) {
    printf("Processando declaracao 'mostrar'...\n");
    Node* node = create_node(*token); // N� da declara��o de sa�da
    token = match(PALAVRA_CHAVE,token);   // "mostrar"
    add_child(node, create_node(*token)); 
    token = match(IDENTIFICADOR,token); // x
    add_child(node, create_node(*token)); 

    token = match(DELIMITADOR,token);     // ";"
    printf("Declaracao 'mostrar' processada com sucesso.\n");
    return node;
}
// <if_statement> ::= "se" "(" <IDENTIFICADOR> "==" <IDENTIFICADOR> ")" "{" <statements> "}"
Node* if_statement(Token* token) {
    printf("Processando declaracao 'if'...\n");
    Node* node = create_node(*token); // N� do if
    token = match(PALAVRA_CHAVE,token);   // "se"
    token = match(DELIMITADOR,token);     // "("
    
    add_child(node, create_node(*token)); // Identificador
    token = match(IDENTIFICADOR,token);   // identificador (x, y)
    token = match(OPERADOR_RELACIONAL,token);        // operador "=="
    add_child(node, create_node(*token)); // Identificador
    token = match(IDENTIFICADOR,token);   // identificador (x, y)
    
    token = match(DELIMITADOR,token);     // ")"
    token = match(DELIMITADOR,token);     // "{"
    
    add_child(node, output_statement(token)); // Adiciona a declara��o de sa�da ao if
    token = match(DELIMITADOR,token);     // "}"
    printf("Declaração 'if' processada com sucesso.\n");
    return node;
}



// <statement> ::= <if_statement> | <output_statement>
Node* statement(Token* token) {
    if (strcmp(token->lexema, "se") == 0) {
        return if_statement(token);
    } else if (strcmp(token->lexema, "mostrar") == 0) {
        return output_statement(token);
    } else {
        printf("Erro na linha %d: statement invalido.\n", token->linha);
        return NULL;
    }
}

// <program> ::= <declaration> <statements>
Node* monta_arvore(Token* currentToken) {
    Node* root = create_node((Token) {PALAVRA_CHAVE, "", 0, 0}); // N� raiz
    while (currentToken->next != NULL) {
        if (currentToken->tipo == PALAVRA_CHAVE && 
            (strcmp(currentToken->lexema, "inteiro") == 0 || 
             strcmp(currentToken->lexema, "real") == 0)) {
            add_child(root, declaration(currentToken));
        } else {
            add_child(root, statement(currentToken));
        }
      currentToken = currentToken->next;  
    }
    return root;
}

// Fun��o para gravar a �rvore em um arquivo
void write_tree(Node* node, int depth) {
   

    // Verifica e escreve o tipo do token
    switch (node->token.tipo) {
        case PALAVRA_CHAVE:
            printf("<keyword> %s </keyword>\n", node->token.lexema);
            break;
        case IDENTIFICADOR:
            printf("<identifier> %s </identifier>\n", node->token.lexema);
            break;
        case NUMERO_INTEIRO:
            printf("<integer> %s </integer>\n", node->token.lexema);
            break;
        case NUMERO_REAL:
            printf("<real> %s </real>\n", node->token.lexema);
            break;
        case DELIMITADOR:
            // N�o escreva delimitadores, ou ajuste conforme necess�rio
            break;
   
        default:
            printf("<unknown> %s </unknown>\n", node->token.lexema);
            break;
    }

    for (int i = 0; i < node->child_count; i++) {
        write_tree(node->children[i], depth + 1);
    }
}

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
        Token* ListaTokens = NULL;

        printf("Lendo o arquivo %s...\n\n", filepath);
        while (fgets(linha, sizeof(linha), arquivo)) {
      
            analisar_lexico(linha, linha_atual,&ListaTokens);
            linha_atual++;
        }
        // Fecha o arquivo após a leitura
        fclose(arquivo);
        printf("Imprimindo tokens:\n\n");
        imprimirTokens(ListaTokens);
        // Exibe o resumo dos tokens
                exibir_resumo_tokens();
        printf("Montando Arvores sintática\n");
        Node* arvre = monta_arvore(ListaTokens);
        write_tree(arvre, 0);

        
    } else {
        printf("Argumento inválido! Uso: %s -p <caminho_do_arquivo>\n", argv[0]);
        return 1;
    }

    return 0;
}
