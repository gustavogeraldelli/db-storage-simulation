/* ==========================================================================
 * Trabalho 01 - Indexação
 * ========================================================================== */
 
/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
 
typedef enum {false, true} bool;
 
/* Tamanho dos campos dos registros */
/* Campos de tamanho fixo */
#define TAM_DATE 9
#define TAM_DATETIME 13
#define TAM_INT_NUMBER 5
#define TAM_FLOAT_NUMBER 14
#define TAM_ID_CURSO 9
#define TAM_ID_USUARIO 12
#define TAM_TELEFONE 12
#define QTD_MAX_CATEGORIAS 3
 
/* Campos de tamanho variável (tamanho máximo) */
#define TAM_MAX_NOME 45
#define TAM_MAX_TITULO 52
#define TAM_MAX_INSTITUICAO 52
#define TAM_MAX_MINISTRANTE 51
#define TAM_MAX_EMAIL 45
#define TAM_MAX_CATEGORIA 21
 
#define MAX_REGISTROS 1000
#define TAM_REGISTRO_USUARIO (TAM_ID_USUARIO+TAM_MAX_NOME+TAM_MAX_EMAIL+TAM_FLOAT_NUMBER+TAM_TELEFONE)
#define TAM_REGISTRO_CURSO (TAM_ID_CURSO+TAM_MAX_TITULO+TAM_MAX_INSTITUICAO+TAM_MAX_MINISTRANTE+TAM_DATE+TAM_INT_NUMBER+TAM_FLOAT_NUMBER+QTD_MAX_CATEGORIAS*TAM_MAX_CATEGORIA+1)
#define TAM_REGISTRO_INSCRICAO (TAM_ID_CURSO+TAM_ID_USUARIO+TAM_DATETIME+1+TAM_DATETIME-4)
#define TAM_ARQUIVO_USUARIOS (TAM_REGISTRO_USUARIO * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_CURSOS (TAM_REGISTRO_CURSO * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_INSCRICOES (TAM_REGISTRO_INSCRICAO * MAX_REGISTROS + 1)
 
#define TAM_RRN_REGISTRO 4
#define TAM_CHAVE_USUARIOS_IDX (TAM_ID_USUARIO + TAM_RRN_REGISTRO - 1)
#define TAM_CHAVE_CURSOS_IDX (TAM_ID_CURSO + TAM_RRN_REGISTRO - 1)
#define TAM_CHAVE_INSCRICOES_IDX (TAM_ID_USUARIO + TAM_ID_CURSO + TAM_RRN_REGISTRO - 2)
#define TAM_CHAVE_TITULO_IDX (TAM_MAX_TITULO + TAM_ID_CURSO - 2)
#define TAM_CHAVE_DATA_USUARIO_CURSO_IDX (TAM_DATETIME + TAM_ID_USUARIO + TAM_ID_CURSO - 3)
#define TAM_CHAVE_CATEGORIAS_SECUNDARIO_IDX (TAM_MAX_CATEGORIA - 1)
#define TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX (TAM_ID_CURSO - 1)
 
#define TAM_ARQUIVO_USUARIOSS_IDX (1000 * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_CURSOSS_IDX (1000 * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_INSCRICOES_IDX (1000 * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_CATEGORIAS_IDX (1000 * MAX_REGISTROS + 1)
 
/* Mensagens padrões */
#define SUCESSO                          "OK\n"
#define REGS_PERCORRIDOS                 "Registros percorridos:"
#define INDICE_CRIADO                    "Indice %s criado com sucesso!\n"
#define AVISO_NENHUM_REGISTRO_ENCONTRADO "AVISO: Nenhum registro encontrado\n"
#define ERRO_OPCAO_INVALIDA              "ERRO: Opcao invalida\n"
#define ERRO_MEMORIA_INSUFICIENTE        "ERRO: Memoria insuficiente\n"
#define ERRO_PK_REPETIDA                 "ERRO: Ja existe um registro com a chave %s\n"
#define ERRO_REGISTRO_NAO_ENCONTRADO     "ERRO: Registro nao encontrado\n"
#define ERRO_SALDO_NAO_SUFICIENTE        "ERRO: Saldo insuficiente\n"
#define ERRO_CATEGORIA_REPETIDA          "ERRO: O curso %s ja possui a categoria %s\n"
#define ERRO_VALOR_INVALIDO              "ERRO: Valor invalido\n"
#define ERRO_ARQUIVO_VAZIO               "ERRO: Arquivo vazio\n"
#define ERRO_NAO_IMPLEMENTADO            "ERRO: Funcao %s nao implementada\n"
 
/* Registro de Usuario */
typedef struct {
    char id_usuario[TAM_ID_USUARIO];
    char nome[TAM_MAX_NOME];
    char email[TAM_MAX_EMAIL];
    char telefone[TAM_TELEFONE];
    double saldo;
} Usuario;
 
/* Registro de Curso */
typedef struct {
    char id_curso[TAM_ID_CURSO];
    char titulo[TAM_MAX_TITULO];
    char instituicao[TAM_MAX_INSTITUICAO];
    char ministrante[TAM_MAX_MINISTRANTE];
    char lancamento[TAM_DATE];
    int carga;
    double valor;
    char categorias[QTD_MAX_CATEGORIAS][TAM_MAX_CATEGORIA];
} Curso;
 
/* Registro de Inscricao */
typedef struct {
    char id_curso[TAM_ID_CURSO];
    char id_usuario[TAM_ID_USUARIO];
    char data_inscricao[TAM_DATETIME];
    char status;
    char data_atualizacao[TAM_DATETIME];		
} Inscricao;
 
 
/*----- Registros dos índices -----*/
 
/* Struct para o índice primário dos usuários */
typedef struct {
    char id_usuario[TAM_ID_USUARIO];
    int rrn;
} usuarios_index;
 
/* Struct para o índice primário dos cursos */
typedef struct {
    char id_curso[TAM_ID_CURSO];
    int rrn;
} cursos_index;
 
/* Struct para índice primário dos inscricoes */
typedef struct {
    char id_curso[TAM_ID_CURSO];
    char id_usuario[TAM_ID_USUARIO];
    int rrn;
} inscricoes_index;
 
/* Struct para o índice secundário dos titulos */
typedef struct {
    char titulo[TAM_MAX_TITULO];
    char id_curso[TAM_ID_CURSO];
} titulos_index;
 
/* Struct para o índice secundário das datas das inscricoes */
typedef struct {
    char data[TAM_DATETIME];
    char id_curso[TAM_ID_CURSO];
    char id_usuario[TAM_ID_USUARIO];
} data_curso_usuario_index;
 
/* Struct para o índice secundário das categorias (lista invertida) */
typedef struct {
    char chave_secundaria[TAM_MAX_CATEGORIA];   //string com o nome da categoria
    int primeiro_indice;
} categorias_secundario_index;
 
/* Struct para o índice primário das categorias (lista invertida) */
typedef struct {
    char chave_primaria[TAM_ID_CURSO];   //string com o id do curso
    int proximo_indice;
} categorias_primario_index;
 
/* Struct para os parâmetros de uma lista invertida */
typedef struct {
    // Ponteiro para o índice secundário
    categorias_secundario_index *categorias_secundario_idx;
 
    // Ponteiro para o arquivo de índice primário
    categorias_primario_index *categorias_primario_idx;
 
    // Quantidade de registros de índice secundário
    unsigned qtd_registros_secundario;
 
    // Quantidade de registros de índice primário
    unsigned qtd_registros_primario;
 
    // Tamanho de uma chave secundária nesse índice
    unsigned tam_chave_secundaria;
 
    // Tamanho de uma chave primária nesse índice
    unsigned tam_chave_primaria;
 
    // Função utilizada para comparar as chaves do índice secundário.
    // Igual às funções de comparação do bsearch e qsort.
    int (*compar)(const void *key, const void *elem);
} inverted_list;
 
/* Variáveis globais */
/* Arquivos de dados */
char ARQUIVO_USUARIOS[TAM_ARQUIVO_USUARIOS];
char ARQUIVO_CURSOS[TAM_ARQUIVO_CURSOS];
char ARQUIVO_INSCRICOES[TAM_ARQUIVO_INSCRICOES];
 
/* Índices */
usuarios_index *usuarios_idx = NULL;
cursos_index *cursos_idx = NULL;
inscricoes_index *inscricoes_idx = NULL;
titulos_index *titulo_idx = NULL;
data_curso_usuario_index *data_curso_usuario_idx = NULL;
inverted_list categorias_idx = {
    .categorias_secundario_idx = NULL,
    .categorias_primario_idx = NULL,
    .qtd_registros_secundario = 0,
    .qtd_registros_primario = 0,
    .tam_chave_secundaria = TAM_CHAVE_CATEGORIAS_SECUNDARIO_IDX,
    .tam_chave_primaria = TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX,
};
 
/* Funções auxiliares para o qsort.
 * Com uma pequena alteração, é possível utilizá-las no bsearch, assim, evitando código duplicado.
 * */
int qsort_usuarios_idx(const void *a, const void *b);
int qsort_cursos_idx(const void *a, const void *b);
int qsort_inscricoes_idx(const void *a, const void *b);
int qsort_titulo_idx(const void *a, const void *b);
int qsort_data_curso_usuario_idx(const void *a, const void *b);
int qsort_data_idx(const void *a, const void *b);
int qsort_categorias_secundario_idx(const void *a, const void *b);
 
/* Contadores */
unsigned qtd_registros_usuarios = 0;
unsigned qtd_registros_cursos = 0;
unsigned qtd_registros_inscricoes = 0;
 
/* Funções de geração determinística de números pseudo-aleatórios */
uint64_t prng_seed;
 
void prng_srand(uint64_t value) {
    prng_seed = value;
}
 
uint64_t prng_rand() {
    // https://en.wikipedia.org/wiki/Xorshift#xorshift*
    uint64_t x = prng_seed; // O estado deve ser iniciado com um valor diferente de 0
    x ^= x >> 12; // a
    x ^= x << 25; // b
    x ^= x >> 27; // c
    prng_seed = x;
    return x * UINT64_C(0x2545F4914F6CDD1D);
}
 
/**
 * Gera um <a href="https://en.wikipedia.org/wiki/Universally_unique_identifier">UUID Version-4 Variant-1</a>
 * (<i>string</i> aleatória) de 36 caracteres utilizando o gerador de números pseudo-aleatórios
 * <a href="https://en.wikipedia.org/wiki/Xorshift#xorshift*">xorshift*</a>. O UUID é
 * escrito na <i>string</i> fornecida como parâmetro.<br />
 * <br />
 * Exemplo de uso:<br />
 * <code>
 * char chave_aleatoria[37];<br />
 * new_uuid(chave_aleatoria);<br />
 * printf("chave aleatória: %s&#92;n", chave_aleatoria);<br />
 * </code>
 *
 * @param buffer String de tamanho 37 no qual será escrito
 * o UUID. É terminado pelo caractere <code>\0</code>.
 */
void new_uuid(char buffer[37]) {
    uint64_t r1 = prng_rand();
    uint64_t r2 = prng_rand();
 
    sprintf(buffer, "%08x-%04x-%04lx-%04lx-%012lx", (uint32_t)(r1 >> 32), (uint16_t)(r1 >> 16), 0x4000 | (r1 & 0x0fff), 0x8000 | (r2 & 0x3fff), r2 >> 16);
}
 
/* Funções de manipulação de data */
time_t epoch;

#define YEAR0                   1900
#define EPOCH_YR                1970
#define SECS_DAY                (24L * 60L * 60L)
#define LEAPYEAR(year)          (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year)          (LEAPYEAR(year) ? 366 : 365)

#define TIME_MAX                2147483647L

long _dstbias = 0;                  // Offset for Daylight Saving Time
long _timezone = 0;                 // Difference in seconds between GMT and local time

const int _ytab[2][12] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

struct tm *gmtime_r(const time_t *timer, struct tm *tmbuf) {
    // based on http://www.jbox.dk/sanos/source/lib/time.c.html
    time_t time = *timer;
    unsigned long dayclock, dayno;
    int year = EPOCH_YR;

    dayclock = (unsigned long) time % SECS_DAY;
    dayno = (unsigned long) time / SECS_DAY;

    tmbuf->tm_sec = dayclock % 60;
    tmbuf->tm_min = (dayclock % 3600) / 60;
    tmbuf->tm_hour = dayclock / 3600;
    tmbuf->tm_wday = (dayno + 4) % 7; // Day 0 was a thursday
    while (dayno >= (unsigned long) YEARSIZE(year)) {
        dayno -= YEARSIZE(year);
        year++;
    }
    tmbuf->tm_year = year - YEAR0;
    tmbuf->tm_yday = dayno;
    tmbuf->tm_mon = 0;
    while (dayno >= (unsigned long) _ytab[LEAPYEAR(year)][tmbuf->tm_mon]) {
        dayno -= _ytab[LEAPYEAR(year)][tmbuf->tm_mon];
        tmbuf->tm_mon++;
    }
    tmbuf->tm_mday = dayno + 1;
    tmbuf->tm_isdst = 0;
    return tmbuf;
}

time_t mktime(struct tm *tmbuf) {
    // based on http://www.jbox.dk/sanos/source/lib/time.c.html
    long day, year;
    int tm_year;
    int yday, month;
    /*unsigned*/ long seconds;
    int overflow;
    long dst;

    tmbuf->tm_min += tmbuf->tm_sec / 60;
    tmbuf->tm_sec %= 60;
    if (tmbuf->tm_sec < 0) {
        tmbuf->tm_sec += 60;
        tmbuf->tm_min--;
    }
    tmbuf->tm_hour += tmbuf->tm_min / 60;
    tmbuf->tm_min = tmbuf->tm_min % 60;
    if (tmbuf->tm_min < 0) {
        tmbuf->tm_min += 60;
        tmbuf->tm_hour--;
    }
    day = tmbuf->tm_hour / 24;
    tmbuf->tm_hour= tmbuf->tm_hour % 24;
    if (tmbuf->tm_hour < 0) {
        tmbuf->tm_hour += 24;
        day--;
    }
    tmbuf->tm_year += tmbuf->tm_mon / 12;
    tmbuf->tm_mon %= 12;
    if (tmbuf->tm_mon < 0) {
        tmbuf->tm_mon += 12;
        tmbuf->tm_year--;
    }
    day += (tmbuf->tm_mday - 1);
    while (day < 0) {
        if(--tmbuf->tm_mon < 0) {
            tmbuf->tm_year--;
            tmbuf->tm_mon = 11;
        }
        day += _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon];
    }
    while (day >= _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon]) {
        day -= _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon];
        if (++(tmbuf->tm_mon) == 12) {
            tmbuf->tm_mon = 0;
            tmbuf->tm_year++;
        }
    }
    tmbuf->tm_mday = day + 1;
    year = EPOCH_YR;
    if (tmbuf->tm_year < year - YEAR0) return (time_t) -1;
    seconds = 0;
    day = 0;                      // Means days since day 0 now
    overflow = 0;

    // Assume that when day becomes negative, there will certainly
    // be overflow on seconds.
    // The check for overflow needs not to be done for leapyears
    // divisible by 400.
    // The code only works when year (1970) is not a leapyear.
    tm_year = tmbuf->tm_year + YEAR0;

    if (TIME_MAX / 365 < tm_year - year) overflow++;
    day = (tm_year - year) * 365;
    if (TIME_MAX - day < (tm_year - year) / 4 + 1) overflow++;
    day += (tm_year - year) / 4 + ((tm_year % 4) && tm_year % 4 < year % 4);
    day -= (tm_year - year) / 100 + ((tm_year % 100) && tm_year % 100 < year % 100);
    day += (tm_year - year) / 400 + ((tm_year % 400) && tm_year % 400 < year % 400);

    yday = month = 0;
    while (month < tmbuf->tm_mon) {
        yday += _ytab[LEAPYEAR(tm_year)][month];
        month++;
    }
    yday += (tmbuf->tm_mday - 1);
    if (day + yday < 0) overflow++;
    day += yday;

    tmbuf->tm_yday = yday;
    tmbuf->tm_wday = (day + 4) % 7;               // Day 0 was thursday (4)

    seconds = ((tmbuf->tm_hour * 60L) + tmbuf->tm_min) * 60L + tmbuf->tm_sec;

    if ((TIME_MAX - seconds) / SECS_DAY < day) overflow++;
    seconds += day * SECS_DAY;

    // Now adjust according to timezone and daylight saving time
    if (((_timezone > 0) && (TIME_MAX - _timezone < seconds)) || 
        ((_timezone < 0) && (seconds < -_timezone))) {
        overflow++;
    }
    seconds += _timezone;

    if (tmbuf->tm_isdst) {
        dst = _dstbias;
    } else {
        dst = 0;
    }

    if (dst > seconds) overflow++;        // dst is always non-negative
    seconds -= dst;

    if (overflow) return (time_t) -1;

    if ((time_t) seconds != seconds) return (time_t) -1;
    return (time_t) seconds;
}
 
bool set_time(char *date) {
    // http://www.cplusplus.com/reference/ctime/mktime/
    struct tm tm_;

    if (strlen(date) == TAM_DATETIME-1 && sscanf(date, "%4d%2d%2d%2d%2d", &tm_.tm_year, &tm_.tm_mon, &tm_.tm_mday, &tm_.tm_hour, &tm_.tm_min) == 5) {
        tm_.tm_year -= 1900;
        tm_.tm_mon -= 1;
        tm_.tm_sec = 0;
        tm_.tm_isdst = -1;
        epoch = mktime(&tm_);
        return true;
    }
    return false;
}
 
void tick_time() {
    epoch += prng_rand() % 864000; // 10 dias
}
 
/**
 * Escreve a <i>data</i> atual no formato <code>AAAAMMDD</code> em uma <i>string</i>
 * fornecida como parâmetro.<br />
 * <br />
 * Exemplo de uso:<br />
 * <code>
 * char timestamp[TAM_DATE];<br />
 * current_date(timestamp);<br />
 * printf("data atual: %s&#92;n", timestamp);<br />
 * </code>
 *
 * @param buffer String de tamanho <code>TAM_DATE</code> no qual será escrita
 * a <i>timestamp</i>. É terminado pelo caractere <code>\0</code>.
 */
void current_date(char buffer[TAM_DATE]) {
    // http://www.cplusplus.com/reference/ctime/strftime/
    // http://www.cplusplus.com/reference/ctime/gmtime/
    // AAAA MM DD
    // %Y   %m %d
    struct tm tm_;
    if (gmtime_r(&epoch, &tm_) != NULL)
        strftime(buffer, TAM_DATE, "%Y%m%d", &tm_);
}
 
/**
 * Escreve a <i>data</i> e a <i>hora</i> atual no formato <code>AAAAMMDDHHMM</code> em uma <i>string</i>
 * fornecida como parâmetro.<br />
 * <br />
 * Exemplo de uso:<br />
 * <code>
 * char timestamp[TAM_DATETIME];<br />
 * current_datetime(timestamp);<br />
 * printf("data e hora atual: %s&#92;n", timestamp);<br />
 * </code>
 *
 * @param buffer String de tamanho <code>TAM_DATETIME</code> no qual será escrita
 * a <i>timestamp</i>. É terminado pelo caractere <code>\0</code>.
 */
void current_datetime(char buffer[TAM_DATETIME]) {
    // http://www.cplusplus.com/reference/ctime/strftime/
    // http://www.cplusplus.com/reference/ctime/gmtime/
    // AAAA MM DD HH MM
    // %Y   %m %d %H %M
    struct tm tm_;
    if (gmtime_r(&epoch, &tm_) != NULL)
        strftime(buffer, TAM_DATETIME, "%Y%m%d%H%M", &tm_);
}

/* Remove comentários (--) e caracteres whitespace do começo e fim de uma string */
void clear_input(char *str) {
    char *ptr = str;
    int len = 0;
 
    for (; ptr[len]; ++len) {
        if (strncmp(&ptr[len], "--", 2) == 0) {
            ptr[len] = '\0';
            break;
        }
    }
 
    while(len-1 > 0 && isspace(ptr[len-1]))
        ptr[--len] = '\0';
 
    while(*ptr && isspace(*ptr))
        ++ptr, --len;
 
    memmove(str, ptr, len + 1);
}
 
 
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */
 
/* Cria o índice respectivo */
void criar_usuarios_idx();
void criar_cursos_idx();
void criar_inscricoes_idx();
void criar_titulo_idx();
void criar_data_curso_usuario_idx();
void criar_categorias_idx();
 
/* Exibe um registro com base no RRN */
bool exibir_usuario(int rrn);
bool exibir_curso(int rrn);
bool exibir_inscricao(int rrn);
 
/* Recupera do arquivo o registro com o RRN informado
 * e retorna os dados nas structs Usuario, Curso e Inscricao */
Usuario recuperar_registro_usuario(int rrn);
Curso recuperar_registro_curso(int rrn);
Inscricao recuperar_registro_inscricao(int rrn);
 
/* Escreve em seu respectivo arquivo na posição informada (RRN) */
void escrever_registro_usuario(Usuario u, int rrn);
void escrever_registro_curso(Curso j, int rrn);
void escrever_registro_inscricao(Inscricao c, int rrn);
 
/* Funções principais */
void cadastrar_usuario_menu(char* id_usuario, char* nome, char* email, char* telefone);
void cadastrar_telefone_menu(char* id_usuario, char* telefone);
void remover_usuario_menu(char *id_usuario);
void cadastrar_curso_menu(char* titulo, char* instituicao, char* ministrante, char* lancamento, int carga, double valor);
void adicionar_saldo_menu(char* id_usuario, double valor);
void inscrever_menu(char *id_curso, char* id_usuario);
void cadastrar_categoria_menu(char* titulo, char* categoria);
void atualizar_status_inscricoes_menu(char* id_usuario, char* titulo, char status);
 
/* Busca */
void buscar_usuario_id_menu(char *id_usuario);
void buscar_curso_id_menu(char *id_curso);
void buscar_curso_titulo_menu(char *titulo);
 
/* Listagem */
void listar_usuarios_id_menu();
void listar_cursos_categorias_menu(char *categoria);
void listar_inscricoes_periodo_menu(char *data_inicio, char *data_fim);
 
/* Liberar espaço */
void liberar_espaco_menu();
 
/* Imprimir arquivos de dados */
void imprimir_arquivo_usuarios_menu();
void imprimir_arquivo_cursos_menu();
void imprimir_arquivo_inscricoes_menu();
 
/* Imprimir índices primários */
void imprimir_usuarios_idx_menu();
void imprimir_cursos_idx_menu();
void imprimir_inscricoes_idx_menu();
 
/* Imprimir índices secundários */
void imprimir_titulo_idx_menu();
void imprimir_data_curso_usuario_idx_menu();
void imprimir_categorias_secundario_idx_menu();
void imprimir_categorias_primario_idx_menu();
 
/* Liberar memória e encerrar programa */
void liberar_memoria_menu();
 
/* Funções de manipulação de Lista Invertida */
/**
 * Responsável por inserir duas chaves (chave_secundaria e chave_primaria) em uma Lista Invertida (t).<br />
 * Atualiza os parâmetros dos índices primário e secundário conforme necessário.<br />
 * As chaves a serem inseridas devem estar no formato correto e com tamanho t->tam_chave_primario e t->tam_chave_secundario.<br />
 * O funcionamento deve ser genérico para qualquer Lista Invertida, adaptando-se para os diferentes parâmetros presentes em seus structs.<br />
 *
 * @param chave_secundaria Chave a ser buscada (caso exista) ou inserida (caso não exista) no registro secundário da Lista Invertida.
 * @param chave_primaria Chave a ser inserida no registro primário da Lista Invertida.
 * @param t Ponteiro para a Lista Invertida na qual serão inseridas as chaves.
 */
void inverted_list_insert(char *chave_secundaria, char *chave_primaria, inverted_list *t);
 
/**
 * Responsável por buscar uma chave no índice secundário de uma Lista invertida (T). O valor de retorno indica se a chave foi encontrada ou não.
 * O ponteiro para o int result pode ser fornecido opcionalmente, e conterá o índice inicial das chaves no registro primário.<br />
 * <br />
 * Exemplos de uso:<br />
 * <code>
 * // Exemplo 1. A chave encontrada deverá ser retornada e o caminho não deve ser informado.<br />
 * ...<br />
 * int result;<br />
 * bool found = inverted_list_secondary_search(&result, false, categoria, &categorias_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 2. Não há interesse na chave encontrada, apenas se ela existe, e o caminho não deve ser informado.<br />
 * ...<br />
 * bool found = inverted_list_secondary_search(NULL, false, categoria, &categorias_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 3. Há interesse no caminho feito para encontrar a chave.<br />
 * ...<br />
 * int result;<br />
 * bool found = inverted_list_secondary_search(&result, true, categoria, &categorias_idx);<br />
 * </code>
 *
 * @param result Ponteiro para ser escrito o índice inicial (primeira ocorrência) das chaves do registro primário. É ignorado caso NULL.
 * @param exibir_caminho Indica se o caminho percorrido deve ser impresso.
 * @param chave_secundaria Chave a ser buscada.
 * @param t Ponteiro para o índice do tipo Lista invertida no qual será buscada a chave.
 * @return Indica se a chave foi encontrada.
 */
bool inverted_list_secondary_search(int *result, bool exibir_caminho, char *chave_secundaria, inverted_list *t);
 
/**
 * Responsável por percorrer o índice primário de uma Lista invertida (T). O valor de retorno indica a quantidade de chaves encontradas.
 * O ponteiro para o vetor de strings result pode ser fornecido opcionalmente, e será populado com a lista de todas as chaves encontradas.
 * O ponteiro para o inteiro indice_final também pode ser fornecido opcionalmente, e deve conter o índice do último campo da lista encadeada 
 * da chave primaria fornecida (isso é útil na inserção de um novo registro).<br />
 * <br />
 * Exemplos de uso:<br />
 * <code>
 * // Exemplo 1. As chaves encontradas deverão ser retornadas e tanto o caminho quanto o indice_final não devem ser informados.<br />
 * ...<br />
 * char chaves[TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX][MAX_REGISTROS];<br />
 * int qtd = inverted_list_primary_search(chaves, false, indice, NULL, &categorias_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 2. Não há interesse nas chaves encontradas, apenas no indice_final, e o caminho não deve ser informado.<br />
 * ...<br />
 * int indice_final;
 * int qtd = inverted_list_primary_search(NULL, false, indice, &indice_final, &categorias_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 3. Há interesse nas chaves encontradas e no caminho feito.<br />
 * ...<br />
 * char chaves[TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX][MAX_REGISTROS];<br />
 * int qtd = inverted_list_primary_search(chaves, true, indice, NULL, &categorias_idx);<br />
 * ...<br />
 * <br />
 * </code>
 *
 * @param result Ponteiro para serem escritas as chaves encontradas. É ignorado caso NULL.
 * @param exibir_caminho Indica se o caminho percorrido deve ser impresso.
 * @param indice Índice do primeiro registro da lista encadeada a ser procurado.
 * @param indice_final Ponteiro para ser escrito o índice do último registro encontrado (cujo campo indice é -1). É ignorado caso NULL.
 * @param t Ponteiro para o índice do tipo Lista invertida no qual será buscada a chave.
 * @return Indica a quantidade de chaves encontradas.
 */
int inverted_list_primary_search(char result[][TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX], bool exibir_caminho, int indice, int *indice_final, inverted_list *t);
 
/**
 * Preenche uma string str com o caractere pad para completar o tamanho size.<br />
 *
 * @param str Ponteiro para a string a ser manipulada.
 * @param pad Caractere utilizado para fazer o preenchimento à direita.
 * @param size Tamanho desejado para a string.
 */
char* strpadright(char *str, char pad, unsigned size);

/**
 * Converte uma string str para letras maiúsculas.<br />
 *
 * @param str Ponteiro para a string a ser manipulada.
 */
char* strupr(char *str);

/**
 * Converte uma string str para letras minúsculas.<br />
 *
 * @param str Ponteiro para a string a ser manipulada.
 */
char* strlower(char *str);
 
/* Funções de busca binária */
/**
 * Função Genérica de busca binária, que aceita parâmetros genéricos (assinatura baseada na função bsearch da biblioteca C).
 * 
 * @param key Chave de busca genérica.
 * @param base0 Base onde ocorrerá a busca, por exemplo, um ponteiro para um vetor.
 * @param nmemb Número de elementos na base.
 * @param size Tamanho do tipo do elemento na base, dica: utilize a função sizeof().
 * @param compar Ponteiro para a função que será utilizada nas comparações.
 * @param exibir_caminho Indica se o caminho percorrido deve ser impresso. Se true, imprime as posições avaliadas durante a busca, até que todas sejam visitadas ou o elemento seja encontrado (pela 1a vez).
 *                       Caso o número de elementos seja par (p.ex, 10 elementos), então há 2 (duas) possibilidades para a posição da mediana dos elementos (p.ex., 5a ou 6a posição se o total fosse 10).
 *                       Neste caso, SEMPRE escolha a posição mais à direita (p.ex., a posição 6 caso o total for 10).
 * @param retorno_se_nao_encontrado Caso o elemento NÃO seja encontrado, indica qual valor deve ser retornado. As opções são:
 *                     -1 = predecessor : retorna o elemento PREDECESSOR (o que apareceria imediatamente antes do elemento procurado, caso fosse encontrado). 
 *                      0 = nulo : retorna NULL. [modo padrão]
 *                     +1 = sucessor : retorna o elemento SUCESSOR (o que apareceria imediatamente depois do elemento procurado, caso fosse encontrado).	
 * @return Retorna o elemento encontrado ou NULL se não encontrou.
 */
void* busca_binaria(const void *key, const void *base0, size_t nmemb, size_t size, int (*compar)(const void *, const void *), bool exibir_caminho, int retorno_se_nao_encontrado);

 
/* <<< COLOQUE AQUI OS DEMAIS PROTÓTIPOS DE FUNÇÕES, SE NECESSÁRIO >>> */


/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
 
int main() {
    // variáveis utilizadas pelo interpretador de comandos
    char input[500];
    uint64_t seed = 2;
    char datetime[TAM_DATETIME] = "202103181430"; // UTC 18/03/2021 14:30:00
    char id_usuario[TAM_ID_USUARIO];
    char nome[TAM_MAX_NOME];
    char email[TAM_MAX_EMAIL];
    char telefone[TAM_TELEFONE];
    char id_curso[TAM_ID_CURSO];
    char titulo[TAM_MAX_TITULO];
    char instituicao[TAM_MAX_INSTITUICAO];
    char ministrante[TAM_MAX_MINISTRANTE];
    char lancamento[TAM_DATE];
    char categoria[TAM_MAX_CATEGORIA];
    int carga;
    double valor;
    char data_inicio[TAM_DATETIME];
    char data_fim[TAM_DATETIME];
    char status;
 
    scanf("SET ARQUIVO_USUARIOS TO '%[^']';\n", ARQUIVO_USUARIOS);
    int temp_len = strlen(ARQUIVO_USUARIOS);
    qtd_registros_usuarios = temp_len / TAM_REGISTRO_USUARIO;
    ARQUIVO_USUARIOS[temp_len] = '\0';
 
    scanf("SET ARQUIVO_CURSOS TO '%[^']';\n", ARQUIVO_CURSOS);
    temp_len = strlen(ARQUIVO_CURSOS);
    qtd_registros_cursos = temp_len / TAM_REGISTRO_CURSO;
    ARQUIVO_CURSOS[temp_len] = '\0';
 
    scanf("SET ARQUIVO_INSCRICOES TO '%[^']';\n", ARQUIVO_INSCRICOES);
    temp_len = strlen(ARQUIVO_INSCRICOES);
    qtd_registros_inscricoes = temp_len / TAM_REGISTRO_INSCRICAO;
    ARQUIVO_INSCRICOES[temp_len] = '\0';
 
    // inicialização do gerador de números aleatórios e função de datas
    prng_srand(seed);
    putenv("TZ=UTC");
    set_time(datetime);
 
    criar_usuarios_idx();
    criar_cursos_idx();
    criar_inscricoes_idx();
    criar_titulo_idx();
    criar_data_curso_usuario_idx();
    criar_categorias_idx();
 
    while (1) {
        fgets(input, 500, stdin);
        printf("%s", input);
        clear_input(input);
 
        if (strcmp("", input) == 0)
            continue; // não avança o tempo nem imprime o comando este seja em branco
 
        /* Funções principais */
        if (sscanf(input, "INSERT INTO usuarios VALUES ('%[^']', '%[^']', '%[^']', '%[^']');", id_usuario, nome, email, telefone) == 4)
            cadastrar_usuario_menu(id_usuario, nome, email, telefone);
        else if (sscanf(input, "INSERT INTO usuarios VALUES ('%[^']', '%[^']', '%[^']');", id_usuario, nome, email) == 3)
            { strcpy(telefone, ""); strpadright(telefone, '*', TAM_TELEFONE-1); cadastrar_usuario_menu(id_usuario, nome, email, telefone); }
        else if (sscanf(input, "UPDATE usuarios SET telefone = '%[^']' WHERE id_usuario = '%[^']';", telefone, id_usuario) == 2)
            cadastrar_telefone_menu(id_usuario, telefone);
        else if (sscanf(input, "DELETE FROM usuarios WHERE id_usuario = '%[^']';", id_usuario) == 1)
            remover_usuario_menu(id_usuario);
        else if (sscanf(input, "INSERT INTO cursos VALUES ('%[^']', '%[^']', '%[^']', '%[^']', %d, %lf);", titulo, instituicao, ministrante, lancamento, &carga, &valor) == 6)
            cadastrar_curso_menu(titulo, instituicao, ministrante, lancamento, carga, valor);
        else if (sscanf(input, "UPDATE usuarios SET saldo = saldo + %lf WHERE id_usuario = '%[^']';", &valor, id_usuario) == 2)
            adicionar_saldo_menu(id_usuario, valor);
        else if (sscanf(input, "INSERT INTO inscricoes VALUES ('%[^']', '%[^']');", id_curso, id_usuario) == 2)
            inscrever_menu(id_curso, id_usuario);
        else if (sscanf(input, "UPDATE cursos SET categorias = array_append(categorias, '%[^']') WHERE titulo = '%[^']';", categoria, titulo) == 2)
            cadastrar_categoria_menu(titulo, categoria);
        else if (sscanf(input, "UPDATE inscricoes SET status = '%c' WHERE id_curso = (SELECT id_curso FROM cursos WHERE titulo = '%[^']') AND id_usuario = '%[^']';", &status, titulo, id_usuario) == 3)
            atualizar_status_inscricoes_menu(id_usuario, titulo, status);
 
        /* Busca */
        else if (sscanf(input, "SELECT * FROM usuarios WHERE id_usuario = '%[^']';", id_usuario) == 1)
            buscar_usuario_id_menu(id_usuario);
        else if (sscanf(input, "SELECT * FROM cursos WHERE id_curso = '%[^']';", id_curso) == 1)
            buscar_curso_id_menu(id_curso);
        else if (sscanf(input, "SELECT * FROM cursos WHERE titulo = '%[^']';", titulo) == 1)
            buscar_curso_titulo_menu(titulo);
 
        /* Listagem */
        else if (strcmp("SELECT * FROM usuarios ORDER BY id_usuario ASC;", input) == 0)
            listar_usuarios_id_menu();
        else if (sscanf(input, "SELECT * FROM cursos WHERE '%[^']' = ANY (categorias) ORDER BY id_curso ASC;", categoria) == 1)
            listar_cursos_categorias_menu(categoria);
        else if (sscanf(input, "SELECT * FROM inscricoes WHERE data_inscricao BETWEEN '%[^']' AND '%[^']' ORDER BY data_inscricao ASC;", data_inicio, data_fim) == 2)
            listar_inscricoes_periodo_menu(data_inicio, data_fim);
 
        /* Liberar espaço */
        else if (strcmp("VACUUM usuarios;", input) == 0)
            liberar_espaco_menu();
 
        /* Imprimir arquivos de dados */
        else if (strcmp("\\echo file ARQUIVO_USUARIOS", input) == 0)
            imprimir_arquivo_usuarios_menu();
        else if (strcmp("\\echo file ARQUIVO_CURSOS", input) == 0)
            imprimir_arquivo_cursos_menu();
        else if (strcmp("\\echo file ARQUIVO_INSCRICOES", input) == 0)
            imprimir_arquivo_inscricoes_menu();
        
        /* Imprimir índices primários */
        else if (strcmp("\\echo index usuarios_idx", input) == 0)
            imprimir_usuarios_idx_menu();
        else if (strcmp("\\echo index cursos_idx", input) == 0)
            imprimir_cursos_idx_menu();
        else if (strcmp("\\echo index inscricoes_idx", input) == 0)
            imprimir_inscricoes_idx_menu();
 
        /* Imprimir índices secundários */
        else if (strcmp("\\echo index titulo_idx", input) == 0)
            imprimir_titulo_idx_menu();
        else if (strcmp("\\echo index data_curso_usuario_idx", input) == 0)
            imprimir_data_curso_usuario_idx_menu();
        else if (strcmp("\\echo index categorias_secundario_idx", input) == 0)
            imprimir_categorias_secundario_idx_menu();
        else if (strcmp("\\echo index categorias_primario_idx", input) == 0)
            imprimir_categorias_primario_idx_menu();
 
        /* Liberar memória eventualmente alocada e encerrar programa */
        else if (strcmp("\\q", input) == 0)
            { liberar_memoria_menu(); return 0; }
        else if (sscanf(input, "SET SRAND %lu;", &seed) == 1)
            { prng_srand(seed); printf(SUCESSO); continue; }
        else if (sscanf(input, "SET TIME '%[^']';", datetime) == 1)
            { if (set_time(datetime)) printf(SUCESSO); else printf(ERRO_VALOR_INVALIDO); continue; }
        else
            printf(ERRO_OPCAO_INVALIDA);
 
        tick_time();
    }
}
 
/* ========================================================================== */
 
/* Cria o índice primário usuarios_idx */
void criar_usuarios_idx() {
    if (!usuarios_idx)
        usuarios_idx = malloc(MAX_REGISTROS * sizeof(usuarios_index));
 
    if (!usuarios_idx) {
        printf(ERRO_MEMORIA_INSUFICIENTE);
        exit(1);
    }
 
    for (unsigned i = 0; i < qtd_registros_usuarios; ++i) {
        Usuario u = recuperar_registro_usuario(i);
 
        if (strncmp(u.id_usuario, "*|", 2) == 0)
            usuarios_idx[i].rrn = -1; // registro excluído
        else
            usuarios_idx[i].rrn = i;
 
        strcpy(usuarios_idx[i].id_usuario, u.id_usuario);
    }
 
    qsort(usuarios_idx, qtd_registros_usuarios, sizeof(usuarios_index), qsort_usuarios_idx);
    printf(INDICE_CRIADO, "usuarios_idx");
}
 
/* Cria o índice primário cursos_idx */
void criar_cursos_idx() {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    if (!cursos_idx)
        cursos_idx = malloc(MAX_REGISTROS * sizeof(cursos_index));

    if (!cursos_idx) {
        printf(ERRO_MEMORIA_INSUFICIENTE);
        exit(1);
    }

    for (int i = 0; i < qtd_registros_cursos; i++) {
        Curso c = recuperar_registro_curso(i);
        cursos_idx[i].rrn = i;
        strcpy(cursos_idx[i].id_curso, c.id_curso);
    }

    qsort(cursos_idx, qtd_registros_cursos, sizeof(cursos_index), qsort_cursos_idx);
    printf(INDICE_CRIADO, "cursos_idx");
}
 
/* Cria o índice primário inscricoes_idx */
void criar_inscricoes_idx() {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    if (!inscricoes_idx)
        inscricoes_idx = malloc(MAX_REGISTROS * sizeof(inscricoes_index));

    if (!inscricoes_idx) {
        printf(ERRO_MEMORIA_INSUFICIENTE);
        exit(1);
    }

    for (int i = 0; i < qtd_registros_inscricoes; i++) {
        Inscricao j = recuperar_registro_inscricao(i);
        inscricoes_idx[i].rrn = i;
        strcpy(inscricoes_idx[i].id_curso, j.id_curso);
        strcpy(inscricoes_idx[i].id_usuario, j.id_usuario);
    }

    qsort(inscricoes_idx, qtd_registros_inscricoes, sizeof(inscricoes_index), qsort_inscricoes_idx);
    printf(INDICE_CRIADO, "inscricoes_idx");
}
 
/* Cria o índice secundário titulo_idx */
void criar_titulo_idx() {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    if (!titulo_idx)
        titulo_idx = malloc(MAX_REGISTROS * sizeof(titulos_index));

    if (!titulo_idx) {
        printf(ERRO_MEMORIA_INSUFICIENTE);
        exit(1);
    }

    for (int i = 0; i < qtd_registros_cursos; i++) {
        Curso c = recuperar_registro_curso(i);
        strcpy(titulo_idx[i].id_curso, c.id_curso);
        strcpy(titulo_idx[i].titulo, strupr(c.titulo));
    }

    qsort(titulo_idx, qtd_registros_cursos, sizeof(titulos_index), qsort_titulo_idx);
    printf(INDICE_CRIADO, "titulo_idx");
}
 
/* Cria o índice secundário data_curso_usuario_idx */
void criar_data_curso_usuario_idx() {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    if (!data_curso_usuario_idx)
        data_curso_usuario_idx = malloc(MAX_REGISTROS * sizeof(data_curso_usuario_index));

    if (!data_curso_usuario_idx) {
        printf(ERRO_MEMORIA_INSUFICIENTE);
        exit(1);
    }

    for (int i = 0; i < qtd_registros_inscricoes; i++) {
        Inscricao j = recuperar_registro_inscricao(i);
        strcpy(data_curso_usuario_idx[i].data, j.data_inscricao);
        strcpy(data_curso_usuario_idx[i].id_curso, j.id_curso);
        strcpy(data_curso_usuario_idx[i].id_usuario, j.id_usuario);
    }
    
    qsort(data_curso_usuario_idx, qtd_registros_inscricoes, sizeof(data_curso_usuario_index), qsort_data_curso_usuario_idx);
    printf(INDICE_CRIADO, "data_curso_usuario_idx");
}
 
/* Cria os índices (secundário e primário) de categorias_idx */
void criar_categorias_idx() {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    if (!categorias_idx.categorias_primario_idx)
        categorias_idx.categorias_primario_idx = malloc(MAX_REGISTROS * sizeof(categorias_primario_index));

    if (!categorias_idx.categorias_secundario_idx)
        categorias_idx.categorias_secundario_idx = malloc(MAX_REGISTROS * sizeof(categorias_secundario_index));

    if (!categorias_idx.categorias_primario_idx || !categorias_idx.categorias_secundario_idx) {
        printf(ERRO_MEMORIA_INSUFICIENTE);
        exit(1);
    }

    for (int i = 0; i < qtd_registros_cursos; i++) {
        Curso c = recuperar_registro_curso(i);
        for (int j = 0; j < QTD_MAX_CATEGORIAS; j++) {
            if (strcmp(c.categorias[j], "null") != 0) // categoria existente
                inverted_list_insert(c.categorias[j], c.id_curso, &categorias_idx);
        }
    }
    printf(INDICE_CRIADO, "categorias_idx");
}
 
 
/* Exibe um usuario dado seu RRN */
bool exibir_usuario(int rrn) {
    if (rrn < 0)
        return false;
 
    Usuario u = recuperar_registro_usuario(rrn);
 
    printf("%s, %s, %s, %s, %.2lf\n", u.id_usuario, u.nome, u.email, u.telefone, u.saldo);
    return true;
}
 
/* Exibe um curso dado seu RRN */
bool exibir_curso(int rrn) {
    if (rrn < 0)
        return false;
 
    Curso j = recuperar_registro_curso(rrn);
 
    printf("%s, %s, %s, %s, %s, %d, %.2lf\n", j.id_curso, j.titulo, j.instituicao, j.ministrante, j.lancamento, j.carga, j.valor);
    return true;
}
 
/* Exibe uma inscricao dado seu RRN */
bool exibir_inscricao(int rrn) {
    if (rrn < 0)
        return false;
 
    Inscricao c = recuperar_registro_inscricao(rrn);
 
    printf("%s, %s, %s, %c, %s\n", c.id_curso, c.id_usuario, c.data_inscricao, c.status, c.data_atualizacao);
 
    return true;
}
 
 
/* Recupera do arquivo de usuários o registro com o RRN
 * informado e retorna os dados na struct Usuario */
Usuario recuperar_registro_usuario(int rrn) {
    Usuario u;
    char temp[TAM_REGISTRO_USUARIO + 1], *p;
    strncpy(temp, ARQUIVO_USUARIOS + (rrn * TAM_REGISTRO_USUARIO), TAM_REGISTRO_USUARIO);
    temp[TAM_REGISTRO_USUARIO] = '\0';
    
    p = strtok(temp, ";");
    strcpy(u.id_usuario, p);
    p = strtok(NULL, ";");
    strcpy(u.nome, p);
    p = strtok(NULL, ";");
    strcpy(u.email, p);
    p = strtok(NULL, ";");
    strcpy(u.telefone, p);
    p = strtok(NULL, ";");
    u.saldo = atof(p);
    p = strtok(NULL, ";");
 
    return u;
}
 
/* Recupera do arquivo de cursos o registro com o RRN
 * informado e retorna os dados na struct Curso */
Curso recuperar_registro_curso(int rrn) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    Curso c;
    char temp[TAM_REGISTRO_CURSO + 1], *p;
    strncpy(temp, ARQUIVO_CURSOS + (rrn * TAM_REGISTRO_CURSO), TAM_REGISTRO_CURSO);
    temp[TAM_REGISTRO_CURSO] = '\0';

    p = strtok(temp, ";");
    strcpy(c.id_curso, p);
    p = strtok(NULL, ";");
    strcpy(c.titulo, p);
    p = strtok(NULL, ";");
    strcpy(c.instituicao, p);
    p = strtok(NULL, ";");
    strcpy(c.ministrante, p);
    p = strtok(NULL, ";");
    strcpy(c.lancamento, p);
    p = strtok(NULL, ";");
    c.carga = atoi(p);
    p = strtok(NULL, ";");
    c.valor = atof(p);
    p = strtok(NULL, ";"); // nesse ponto, p = xxx|xxx|xxx|
    // separando as categorias nos '|'
    p = strtok(p, "|");
    for (int i = 0; i < QTD_MAX_CATEGORIAS; i++) {
        if (p == NULL || !strncmp(p, "#", 1)) 
            strcpy(c.categorias[i], "null"); // atribuindo valor especial para campos nao fornecidos
        else
            strcpy(c.categorias[i], p);
        p = strtok(NULL, "|");
    }

    return c;
}
 
/* Recupera do arquivo de inscricoes o registro com o RRN
 * informado e retorna os dados na struct Inscricao */
Inscricao recuperar_registro_inscricao(int rrn) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    Inscricao i;
    char temp[TAM_REGISTRO_INSCRICAO + 1];
    strncpy(temp, ARQUIVO_INSCRICOES + (rrn * TAM_REGISTRO_INSCRICAO), TAM_REGISTRO_INSCRICAO);
    temp[TAM_REGISTRO_INSCRICAO] = '\0';

    strncpy(i.id_curso, temp, 8);
    i.id_curso[TAM_ID_CURSO-1] = '\0';
    strncpy(i.id_usuario, temp + 8, 11);
    i.id_usuario[TAM_ID_USUARIO-1] = '\0';
    strncpy(i.data_inscricao, temp + 19, 12);
    i.data_inscricao[TAM_DATETIME-1] = '\0';
    i.status = (temp + 31)[0];
    strncpy(i.data_atualizacao, temp + 32, 12);
    i.data_atualizacao[TAM_DATETIME-1] = '\0';

    return i;
}
 
 
/* Escreve no arquivo de usuários na posição informada (RRN)
 * os dados na struct Usuario */
void escrever_registro_usuario(Usuario u, int rrn) {
    char temp[TAM_REGISTRO_USUARIO + 1], p[100];
    temp[0] = '\0'; p[0] = '\0';
 
    strcpy(temp, u.id_usuario);
    strcat(temp, ";");
    strcat(temp, u.nome);
    strcat(temp, ";");
    strcat(temp, u.email);
    strcat(temp, ";");
    strcat(temp, u.telefone);
    strcat(temp, ";");
    sprintf(p, "%013.2lf", u.saldo);
    strcat(temp, p);
    strcat(temp, ";");
 
    strpadright(temp, '#', TAM_REGISTRO_USUARIO);

    strncpy(ARQUIVO_USUARIOS + rrn*TAM_REGISTRO_USUARIO, temp, TAM_REGISTRO_USUARIO);
}
 
/* Escreve no arquivo de cursos na posição informada (RRN)
 * os dados na struct Curso */
void escrever_registro_curso(Curso j, int rrn) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    char temp[TAM_REGISTRO_CURSO + 1], p[100];
    temp[0] = '\0'; p[0] = '\0';

    strcpy(temp, j.id_curso);
    strcat(temp, ";");
    strcat(temp, j.titulo);
    strcat(temp, ";");
    strcat(temp, j.instituicao);
    strcat(temp, ";");
    strcat(temp, j.ministrante);
    strcat(temp, ";");
    strcat(temp, j.lancamento);
    strcat(temp, ";");
    sprintf(p, "%04d", j.carga);
    strcat(temp, p);
    strcat(temp, ";");
    sprintf(p, "%013.2lf", j.valor);
    strcat(temp, p);
    strcat(temp, ";");
    for (int i = 0; i < QTD_MAX_CATEGORIAS; i++) {
        if (strcmp(j.categorias[i], "null") != 0) 
            strcat(temp, j.categorias[i]);
        if (i + 1 < QTD_MAX_CATEGORIAS - 1 && strcmp(j.categorias[i+1], "null") != 0) // adiciona "|" se possuir uma categoria na posição a frente
            strcat(temp, "|");
        if (i == QTD_MAX_CATEGORIAS - 1 && strcmp(j.categorias[i], "null") != 0) // adiciona "|" se possuir as 3 categorias
            strcat(temp, "|");
    }
    strcat(temp, ";");

    strpadright(temp, '#', TAM_REGISTRO_CURSO);

    strncpy(ARQUIVO_CURSOS + rrn*TAM_REGISTRO_CURSO, temp, TAM_REGISTRO_CURSO);
}
 
/* Escreve no arquivo de inscricoes na posição informada (RRN)
 * os dados na struct Inscricao */
void escrever_registro_inscricao(Inscricao c, int rrn) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    char temp[TAM_REGISTRO_INSCRICAO + 1];
    temp[0] = '\0';
 
    strcpy(temp, c.id_curso);
    strcat(temp, c.id_usuario);
    strcat(temp, c.data_inscricao);
    temp[31] = c.status;
    temp[32] = '\0';
    strcat(temp, c.data_atualizacao);
    temp[TAM_REGISTRO_INSCRICAO] = '\0';
 
    strncpy(ARQUIVO_INSCRICOES + rrn*TAM_REGISTRO_INSCRICAO, temp, TAM_REGISTRO_INSCRICAO);
}
 
 
/* Funções principais */
void cadastrar_usuario_menu(char *id_usuario, char *nome, char *email, char *telefone) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    usuarios_index chave_usuario; // chave para busca em usuarios_idx
    strcpy(chave_usuario.id_usuario, id_usuario);

    usuarios_index *usuario_buscado = (usuarios_index*) busca_binaria(&chave_usuario, usuarios_idx, qtd_registros_usuarios, sizeof(usuarios_index), qsort_usuarios_idx, false, 0);

    if (usuario_buscado && usuario_buscado->rrn != -1) {
        printf(ERRO_PK_REPETIDA, id_usuario);
        return;
    }

    int novo_rrn = strlen(ARQUIVO_USUARIOS) / TAM_REGISTRO_USUARIO;

    if (usuario_buscado && usuario_buscado->rrn == -1) 
        usuario_buscado->rrn = novo_rrn;
    else {
        chave_usuario.rrn = novo_rrn;
        usuarios_idx[qtd_registros_usuarios] = chave_usuario;
        qtd_registros_usuarios++;
        qsort(usuarios_idx, qtd_registros_usuarios, sizeof(usuarios_index), qsort_usuarios_idx);
    }

    Usuario u;
    strcpy(u.id_usuario, id_usuario);
    strcpy(u.nome, nome);
    strcpy(u.email, email);
    strcpy(u.telefone, telefone);
    u.saldo = 0.0;

    escrever_registro_usuario(u, novo_rrn);
    printf(SUCESSO);
}
 
void cadastrar_telefone_menu(char* id_usuario, char* telefone) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    usuarios_index chave_usuario; // chave para busca em usuarios_idx
    strcpy(chave_usuario.id_usuario, id_usuario);

    usuarios_index *usuario_buscado = (usuarios_index*) busca_binaria(&chave_usuario, usuarios_idx, qtd_registros_usuarios, sizeof(usuarios_index), qsort_usuarios_idx, false, 0);

    if (!usuario_buscado || usuario_buscado->rrn == -1)
        printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    else {
        Usuario u = recuperar_registro_usuario(usuario_buscado->rrn);
        strcpy(u.telefone, telefone);
        escrever_registro_usuario(u, usuario_buscado->rrn);
        printf(SUCESSO);
    }
}
 
void remover_usuario_menu(char *id_usuario) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    usuarios_index chave_usuario; // chave para busca em usuarios_idx
    strcpy(chave_usuario.id_usuario, id_usuario);

    usuarios_index *usuario_buscado = (usuarios_index*) busca_binaria(&chave_usuario, usuarios_idx, qtd_registros_usuarios, sizeof(usuarios_index), qsort_usuarios_idx, false, 0);

    if (!usuario_buscado || usuario_buscado->rrn == -1)
        printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    else {
        Usuario u = recuperar_registro_usuario(usuario_buscado->rrn);
        strncpy(u.id_usuario, "*|", 2);
        escrever_registro_usuario(u, usuario_buscado->rrn);
        usuario_buscado->rrn = -1;
        printf(SUCESSO);
    }
}
 
void cadastrar_curso_menu(char *titulo, char *instituicao, char *ministrante, char* lancamento, int carga, double valor) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    char titulo_normal[TAM_MAX_TITULO];
    strcpy(titulo_normal, titulo); // função strupr transforma o titulo fornecido para caixa alta, então é preciso guardar o titulo real em outra variavel

    strupr(titulo);

    titulos_index chave_titulo; // chave para busca em titulo_idx
    strcpy(chave_titulo.titulo, titulo);

    titulos_index *titulo_buscado = (titulos_index*) busca_binaria(&chave_titulo, titulo_idx, qtd_registros_cursos, sizeof(titulos_index), qsort_titulo_idx, false, 0);

    if (titulo_buscado)
        printf(ERRO_PK_REPETIDA, titulo_normal);
    else {
        char id_curso[TAM_ID_CURSO];
        id_curso[0] = '\0';
        sprintf(id_curso, "%08d", qtd_registros_cursos);
        id_curso[TAM_ID_CURSO - 1] = '\0';

        strcpy(chave_titulo.id_curso, id_curso);

        cursos_index novo_index_curso;
        strcpy(novo_index_curso.id_curso, id_curso);
        novo_index_curso.rrn = qtd_registros_cursos;

        cursos_idx[qtd_registros_cursos] = novo_index_curso;
        titulo_idx[qtd_registros_cursos] = chave_titulo;
        qtd_registros_cursos++;

        Curso c;
        strcpy(c.id_curso, id_curso);
        strcpy(c.titulo, titulo_normal);
        strcpy(c.instituicao, instituicao);
        strcpy(c.ministrante, ministrante);
        strcpy(c.lancamento, lancamento);
        c.carga = carga;
        c.valor = valor;
        for (int i = 0; i < QTD_MAX_CATEGORIAS; i++) 
            strcpy(c.categorias[i], "null"); // marcador para indicar que não há categoria
        
        escrever_registro_curso(c, novo_index_curso.rrn);
        qsort(cursos_idx, qtd_registros_cursos, sizeof(cursos_index), qsort_cursos_idx);
        qsort(titulo_idx, qtd_registros_cursos, sizeof(titulos_index), qsort_titulo_idx);
        printf(SUCESSO);
    }
}
 
void adicionar_saldo_menu(char *id_usuario, double valor) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    if (valor <= 0) {
        printf(ERRO_VALOR_INVALIDO);
        return;
    }

    usuarios_index chave_usuario; // chave para busca em usuarios_idx
    strcpy(chave_usuario.id_usuario, id_usuario);

    usuarios_index *usuario_buscado = (usuarios_index*) busca_binaria(&chave_usuario, usuarios_idx, qtd_registros_usuarios, sizeof(usuarios_index), qsort_usuarios_idx, false, 0);

    if (!usuario_buscado || usuario_buscado->rrn == -1)
        printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    else {
        Usuario u = recuperar_registro_usuario(usuario_buscado->rrn);
        u.saldo += valor;
        escrever_registro_usuario(u, usuario_buscado->rrn);
        printf(SUCESSO);
    }
}
 
 
void inscrever_menu(char *id_curso, char *id_usuario) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    usuarios_index chave_usuario; // chave para busca em usuarios_idx
    cursos_index chave_curso; // chave para busca em cursos_idx
    strcpy(chave_usuario.id_usuario, id_usuario);
    strcpy(chave_curso.id_curso, id_curso);
    
    usuarios_index *usuario_buscado = (usuarios_index*) busca_binaria(&chave_usuario, usuarios_idx, qtd_registros_usuarios, sizeof(usuarios_index), qsort_usuarios_idx, false, 0);
    cursos_index *curso_buscado = (cursos_index*) busca_binaria(&chave_curso, cursos_idx, qtd_registros_cursos, sizeof(cursos_index), qsort_cursos_idx, false, 0);

    if (!usuario_buscado || usuario_buscado->rrn == -1 || !curso_buscado) {
        printf(ERRO_REGISTRO_NAO_ENCONTRADO);
        return;
    }

    Usuario u = recuperar_registro_usuario(usuario_buscado->rrn);
    Curso c = recuperar_registro_curso(curso_buscado->rrn);

    inscricoes_index chave_inscricao; // chave para busca em inscricoes_idx
    strcpy(chave_inscricao.id_curso, id_curso);
    strcpy(chave_inscricao.id_usuario, id_usuario);

    inscricoes_index *inscricao_buscada = (inscricoes_index*) busca_binaria(&chave_inscricao, inscricoes_idx, qtd_registros_inscricoes, sizeof(inscricoes_index), qsort_inscricoes_idx, false, 0);

    if (inscricao_buscada) {
        char chave_inscricao[TAM_ID_USUARIO + TAM_ID_CURSO - 1];
        chave_inscricao[0] = '\0';
        strcpy(chave_inscricao, id_curso);
        strcat(chave_inscricao, id_usuario);
        printf(ERRO_PK_REPETIDA, chave_inscricao);
    }
    else if (u.saldo < c.valor)
        printf(ERRO_SALDO_NAO_SUFICIENTE);
    else {
        u.saldo -= c.valor;
        escrever_registro_usuario(u, usuario_buscado->rrn);

        chave_inscricao.rrn = qtd_registros_inscricoes;
        inscricoes_idx[qtd_registros_inscricoes] = chave_inscricao;

        char data[TAM_DATETIME];
        current_datetime(data);

        data_curso_usuario_index novo_index_dcu;
        strcpy(novo_index_dcu.data, data);
        strcpy(novo_index_dcu.id_curso, id_curso);
        strcpy(novo_index_dcu.id_usuario, id_usuario);

        data_curso_usuario_idx[qtd_registros_inscricoes] = novo_index_dcu;
        qtd_registros_inscricoes++;

        Inscricao i;
        strcpy(i.id_curso, id_curso);
        strcpy(i.id_usuario, id_usuario);
        i.status = 'A';
        strcpy(i.data_inscricao, data);
        strcpy(i.data_atualizacao, data);

        escrever_registro_inscricao(i, chave_inscricao.rrn);
        qsort(inscricoes_idx, qtd_registros_inscricoes, sizeof(inscricoes_index), qsort_inscricoes_idx);
        qsort(data_curso_usuario_idx, qtd_registros_inscricoes, sizeof(data_curso_usuario_index), qsort_data_curso_usuario_idx);
        printf(SUCESSO);
    }
}
 
 
void cadastrar_categoria_menu(char* titulo, char* categoria) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    titulos_index chave_titulo; // chave para busca em titulo_idx
    strcpy(chave_titulo.titulo, strupr(titulo));

    titulos_index *titulo_buscado = (titulos_index*) busca_binaria(&chave_titulo, titulo_idx, qtd_registros_cursos, sizeof(titulos_index), qsort_titulo_idx, false, 0);

    if (!titulo_buscado)
        printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    else {
        cursos_index chave_curso; // chave para busca em cursos_idx
        strcpy(chave_curso.id_curso, titulo_buscado->id_curso); // utilizando id_curso encontrado no vetor titulo_idx

        cursos_index *curso_buscado = (cursos_index*) busca_binaria(&chave_curso, cursos_idx, qtd_registros_cursos, sizeof(cursos_index), qsort_cursos_idx, false, 0);

        Curso c = recuperar_registro_curso(curso_buscado->rrn);

        for (int i = 0; i < QTD_MAX_CATEGORIAS; i++) {
            if (!strcmp(c.categorias[i], categoria)) {
                printf(ERRO_CATEGORIA_REPETIDA, c.titulo, c.categorias[i]);
                return;
            }
            else if (!strcmp(c.categorias[i], "null")) {
                strcpy(c.categorias[i], categoria);
                inverted_list_insert(categoria, c.id_curso, &categorias_idx);
                break;
            }
        }

        escrever_registro_curso(c, curso_buscado->rrn);
        printf(SUCESSO);
    }
}
 
 
void atualizar_status_inscricoes_menu(char *id_usuario, char *titulo, char status) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    titulos_index chave_titulo; // chave para busca em titulo_idx
    strcpy(chave_titulo.titulo, strupr(titulo));

    titulos_index *titulo_buscado = (titulos_index*) busca_binaria(&chave_titulo, titulo_idx, qtd_registros_cursos, sizeof(titulos_index), qsort_titulo_idx, false, 0);

    if (!titulo_buscado) {
        printf(ERRO_REGISTRO_NAO_ENCONTRADO);
        return;
    }

    inscricoes_index chave_inscricao; // chave para busca em inscricoes_idx
    strcpy(chave_inscricao.id_usuario, id_usuario);
    strcpy(chave_inscricao.id_curso, titulo_buscado->id_curso); // utilizando id_curso encontrado no vetor titulo_idx

    inscricoes_index *inscricao_buscada = (inscricoes_index*) busca_binaria(&chave_inscricao, inscricoes_idx, qtd_registros_inscricoes, sizeof(inscricoes_index), qsort_inscricoes_idx, false, 0);

    if (!inscricao_buscada)
        printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    else {
        Inscricao i = recuperar_registro_inscricao(inscricao_buscada->rrn);
        i.status = status;
        char data[TAM_DATETIME];
        current_datetime(data);
        strcpy(i.data_atualizacao, data);
        escrever_registro_inscricao(i, inscricao_buscada->rrn);
        printf(SUCESSO);
    }
}


/* Busca */
void buscar_usuario_id_menu(char *id_usuario) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    usuarios_index chave_usuario; // chave para busca em usuarios_idx
    strcpy(chave_usuario.id_usuario, id_usuario);

    usuarios_index *usuario_buscado = (usuarios_index*) busca_binaria(&chave_usuario, usuarios_idx, qtd_registros_usuarios, sizeof(usuarios_index), qsort_usuarios_idx, true, 0);

    if (!usuario_buscado || usuario_buscado->rrn == -1)
        printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    else 
        exibir_usuario(usuario_buscado->rrn);

}
 
void buscar_curso_id_menu(char *id_curso) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    cursos_index chave_curso; // chave para busca em cursos_idx
    strcpy(chave_curso.id_curso, id_curso);

    cursos_index *curso_buscado = (cursos_index*) busca_binaria(&chave_curso, cursos_idx, qtd_registros_cursos, sizeof(cursos_index), qsort_cursos_idx, true, 0);

    if (!curso_buscado)
        printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    else 
        exibir_curso(curso_buscado->rrn);
}
 
void buscar_curso_titulo_menu(char *titulo) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    titulos_index chave_usuario; // chave para busca em titulo_idx
    strcpy(chave_usuario.titulo, strupr(titulo));

    titulos_index *titulo_buscado = (titulos_index*) busca_binaria(&chave_usuario, titulo_idx, qtd_registros_cursos, sizeof(titulos_index), qsort_titulo_idx, true, 0);

    if (!titulo_buscado) {
        printf(ERRO_REGISTRO_NAO_ENCONTRADO);
        return;
    }

    cursos_index chave_curso; // chave para busca em cursos_idx
    strcpy(chave_curso.id_curso, titulo_buscado->id_curso); // utilizando id_curso encontrado no vetor titulo_idx

    cursos_index *curso_buscado = (cursos_index*) busca_binaria(&chave_curso, cursos_idx, qtd_registros_cursos, sizeof(cursos_index), qsort_cursos_idx, true, 0);

    exibir_curso(curso_buscado->rrn);
}
 
 
/* Listagem */
void listar_usuarios_id_menu() {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    if (qtd_registros_usuarios == 0) {
        printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
        return;
    }

    for (int i = 0; i < qtd_registros_usuarios; i++) {
        Usuario u = recuperar_registro_usuario(i);
        if (!strncmp(u.id_usuario, "*|", 2)) // registro excluido
            continue;
        else
            exibir_usuario(usuarios_idx[i].rrn);
    }
}
 
void listar_cursos_categorias_menu(char *categoria) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    if (qtd_registros_cursos == 0) {
        printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
        return;
    }

    int primeiro_indice;
    bool existe_categoria = inverted_list_secondary_search(&primeiro_indice, false, strupr(categoria), &categorias_idx);

    if (!existe_categoria)
        printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
    else {
        int qtd_cursos_na_categoria = inverted_list_primary_search(NULL, false, primeiro_indice, NULL, &categorias_idx); // contagem para criação de um vetor de tamanho correto
        char ids[qtd_cursos_na_categoria][TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX];
        
        inverted_list_primary_search(ids, true, primeiro_indice, NULL, &categorias_idx); // populando o vetor 'ids' criado

        cursos_index temp[qtd_cursos_na_categoria]; // usado para ser possivel a ordenacao do vetor acima utilizando somente as funções qsort fornecidas

        for (int i = 0; i < qtd_cursos_na_categoria; i++) { 
            strncpy(temp[i].id_curso, ids[i], 8);
            temp[i].id_curso[TAM_ID_CURSO-1] = '\0';
        }

        qsort(temp, qtd_cursos_na_categoria, sizeof(cursos_index), qsort_cursos_idx); // ordenação dos id_curso recuperados

        for (int i = 0; i < qtd_cursos_na_categoria; i++) {
            cursos_index chave_curso; // chave para busca em cursos_idx
            strcpy(chave_curso.id_curso, temp[i].id_curso);
            cursos_index *curso_buscado = (cursos_index*) busca_binaria(&chave_curso, cursos_idx, qtd_registros_cursos, sizeof(cursos_index), qsort_cursos_idx, false, 0);
            exibir_curso(curso_buscado->rrn);
        }
    }
}
 
void listar_inscricoes_periodo_menu(char *data_inicio, char *data_fim) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    if (qtd_registros_inscricoes == 0) {
        printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
        return;
    }

    data_curso_usuario_index chave_data_curso_usuario; // chave para busca em data_curso_usuario_idx
    strcpy(chave_data_curso_usuario.data, data_inicio);

    data_curso_usuario_index *data_buscada = (data_curso_usuario_index*) busca_binaria(&chave_data_curso_usuario, data_curso_usuario_idx, qtd_registros_inscricoes, sizeof(data_curso_usuario_index), qsort_data_idx, true, 1);

    // se a data nao existe ou se a proxima existente é maior que data_fim, retorna
    if (!data_buscada || qsort_data_idx(data_buscada->data, data_fim) > 0) {
        printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
        return;
    }

    // percorrendo vetor de datas até data_fim
    while (qsort_data_idx(data_buscada->data, data_fim) <= 0) {
        inscricoes_index chave_inscricao; // chave para busca em inscricoes_idx
        strcpy(chave_inscricao.id_curso, data_buscada->id_curso);
        strcpy(chave_inscricao.id_usuario, data_buscada->id_usuario);

        inscricoes_index *inscricao_buscada = (inscricoes_index*) busca_binaria(&chave_inscricao, inscricoes_idx, qtd_registros_inscricoes, sizeof(inscricoes_index), qsort_inscricoes_idx, false, 0);
        // como a data existe no vetor das datas, garantidamente uma inscricao existe com a mesma, sendo desnecessario checar um resultado NULL
        exibir_inscricao(inscricao_buscada->rrn);

        if (strcmp(data_buscada->data, data_curso_usuario_idx[qtd_registros_inscricoes-1].data) == 0) // se chegou até o ultimo registro disponivel, não avançar mais posições
            break;

        data_buscada += 1; // avançando mais uma posicao a partir do ponteiro dado pela busca_binaria
    }
}
 
 
/* Liberar espaço */
void liberar_espaco_menu() {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    int qtd_antiga = strlen(ARQUIVO_USUARIOS) / TAM_REGISTRO_USUARIO;
    int nova_qtd = 0;

    for (int i = 0; i < qtd_antiga; i++) {
        Usuario u = recuperar_registro_usuario(i);
        if (!strncmp(u.id_usuario, "*|", 2))
            continue;
        else {
            usuarios_index chave_usuario;
            strcpy(chave_usuario.id_usuario, u.id_usuario);
            chave_usuario.rrn = nova_qtd;
            usuarios_idx[nova_qtd] = chave_usuario;
            nova_qtd++;
            escrever_registro_usuario(u, chave_usuario.rrn);
        }
    }

    qtd_registros_usuarios = nova_qtd;

    qsort(usuarios_idx, qtd_registros_usuarios, sizeof(usuarios_index), qsort_usuarios_idx);
    ARQUIVO_USUARIOS[qtd_registros_usuarios*TAM_REGISTRO_USUARIO] = '\0';
    printf(SUCESSO);
}
 
 
/* Imprimir arquivos de dados */
void imprimir_arquivo_usuarios_menu() {
    if (qtd_registros_usuarios == 0)
        printf(ERRO_ARQUIVO_VAZIO);
    else 
        printf("%s\n", ARQUIVO_USUARIOS);
}
 
void imprimir_arquivo_cursos_menu() {
    if (qtd_registros_cursos == 0)
        printf(ERRO_ARQUIVO_VAZIO);
    else
        printf("%s\n", ARQUIVO_CURSOS);
}
 
void imprimir_arquivo_inscricoes_menu() {
    if (qtd_registros_inscricoes == 0)
        printf(ERRO_ARQUIVO_VAZIO);
    else
        printf("%s\n", ARQUIVO_INSCRICOES);
}
 
 
/* Imprimir índices primários */
void imprimir_usuarios_idx_menu() {
    if (usuarios_idx == NULL || qtd_registros_usuarios == 0)
        printf(ERRO_ARQUIVO_VAZIO);
    else
        for (unsigned i = 0; i < qtd_registros_usuarios; ++i)
            printf("%s, %d\n", usuarios_idx[i].id_usuario, usuarios_idx[i].rrn);
}
 
void imprimir_cursos_idx_menu() {
    if (cursos_idx == NULL || qtd_registros_cursos == 0)
        printf(ERRO_ARQUIVO_VAZIO);
    else
        for (unsigned i = 0; i < qtd_registros_cursos; ++i)
            printf("%s, %d\n", cursos_idx[i].id_curso, cursos_idx[i].rrn);
}
 
void imprimir_inscricoes_idx_menu() {
    if (inscricoes_idx == NULL || qtd_registros_inscricoes == 0)
        printf(ERRO_ARQUIVO_VAZIO);
    else
        for (unsigned i = 0; i < qtd_registros_inscricoes; ++i)
            printf("%s, %s, %d\n", inscricoes_idx[i].id_curso, inscricoes_idx[i].id_usuario, inscricoes_idx[i].rrn);
}
 
 
/* Imprimir índices secundários */
void imprimir_titulo_idx_menu() {
    if (titulo_idx == NULL || qtd_registros_cursos == 0)
        printf(ERRO_ARQUIVO_VAZIO);
    else
        for (unsigned i = 0; i < qtd_registros_cursos; ++i)
            printf("%s, %s\n", titulo_idx[i].titulo, titulo_idx[i].id_curso);
}
 
void imprimir_data_curso_usuario_idx_menu() {
    if (data_curso_usuario_idx == NULL || qtd_registros_inscricoes == 0)
        printf(ERRO_ARQUIVO_VAZIO);
    else     
        for (unsigned i = 0; i < qtd_registros_inscricoes; ++i)
            printf("%s, %s, %s\n", data_curso_usuario_idx[i].data, data_curso_usuario_idx[i].id_curso, data_curso_usuario_idx[i].id_usuario);
}
 
void imprimir_categorias_secundario_idx_menu() {
    if (categorias_idx.categorias_secundario_idx == NULL || categorias_idx.qtd_registros_secundario == 0)
        printf(ERRO_ARQUIVO_VAZIO);
    else
        for (unsigned i = 0; i < categorias_idx.qtd_registros_secundario; ++i)
            printf("%s, %d\n", (categorias_idx.categorias_secundario_idx)[i].chave_secundaria, (categorias_idx.categorias_secundario_idx)[i].primeiro_indice);
}
 
void imprimir_categorias_primario_idx_menu() {
    if (categorias_idx.categorias_primario_idx == NULL || categorias_idx.qtd_registros_primario == 0)
        printf(ERRO_ARQUIVO_VAZIO);
    else
        for (unsigned i = 0; i < categorias_idx.qtd_registros_primario; ++i)
            printf("%s, %d\n", (categorias_idx.categorias_primario_idx)[i].chave_primaria, (categorias_idx.categorias_primario_idx)[i].proximo_indice);
}
 
 
/* Liberar memória e encerrar programa */
void liberar_memoria_menu() {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    free(usuarios_idx);
    free(cursos_idx);
    free(inscricoes_idx);
    free(titulo_idx);
    free(data_curso_usuario_idx);
    free(categorias_idx.categorias_primario_idx);
    free(categorias_idx.categorias_secundario_idx);
}
 
 
/* Função de comparação entre chaves do índice usuarios_idx */
int qsort_usuarios_idx(const void *a, const void *b) {
    return strcmp( ( (usuarios_index *)a )->id_usuario, ( (usuarios_index *)b )->id_usuario);
}
 
/* Função de comparação entre chaves do índice cursos_idx */
int qsort_cursos_idx(const void *a, const void *b) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    return strcmp( ( (cursos_index *)a )->id_curso, ( (cursos_index *)b )->id_curso);
}
 
/* Função de comparação entre chaves do índice inscricoes_idx */
int qsort_inscricoes_idx(const void *a, const void *b) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    int cmp = strcmp( ( (inscricoes_index *)a )->id_curso, ( (inscricoes_index *)b )->id_curso);
    if (cmp == 0) // se ids curso são iguais, ordenar pelo id usuario
        return strcmp( ( (inscricoes_index *)a )->id_usuario, ( (inscricoes_index *)b )->id_usuario);
    return cmp;
}
 
/* Função de comparação entre chaves do índice titulo_idx */
int qsort_titulo_idx(const void *a, const void *b) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    return strcmp( ( (titulos_index *)a )->titulo, ( (titulos_index *)b )->titulo);
}
 
/* Funções de comparação entre chaves do índice data_curso_usuario_idx */
int qsort_data_idx(const void *a, const void *b) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    return strcmp( (char*)a, (char*)b );
}
 
int qsort_data_curso_usuario_idx(const void *a, const void *b) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    int cmp = strcmp( ( (data_curso_usuario_index *)a )->data, ( (data_curso_usuario_index *)b )->data);
    if (cmp != 0)
        return cmp;

    cmp = strcmp( ( (data_curso_usuario_index *)a )->id_curso, ( (data_curso_usuario_index *)b )->id_curso);
    if (cmp != 0)
        return cmp;

    return strcmp( ( (data_curso_usuario_index *)a )->id_usuario, ( (data_curso_usuario_index *)b )->id_usuario);
}
 
/* Função de comparação entre chaves do índice secundário de categorias_idx */
int qsort_categorias_secundario_idx(const void *a, const void *b) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    return strcmp( ( (categorias_secundario_index*)a )->chave_secundaria, ( (categorias_secundario_index*)b )->chave_secundaria);
}
 
 
/* Funções de manipulação de Lista Invertida */
void inverted_list_insert(char *chave_secundaria, char *chave_primaria, inverted_list *t) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    int primeiro_indice;
    bool existe_categoria = inverted_list_secondary_search(&primeiro_indice, false, chave_secundaria, t);

    if (existe_categoria) {
        int ultimo_indice;
        inverted_list_primary_search(NULL, false, primeiro_indice, &ultimo_indice, t); // buscando o ultimo indice
        t->categorias_primario_idx[ultimo_indice].proximo_indice = t->qtd_registros_primario; // o ultimo curso adicionado à categoria fornecida aponta para esse novo curso sendo inserido

        categorias_primario_index novo_index_cat_pri;
        strcpy(novo_index_cat_pri.chave_primaria, chave_primaria);
        novo_index_cat_pri.proximo_indice = -1;

        t->categorias_primario_idx[t->qtd_registros_primario] = novo_index_cat_pri;
        t->qtd_registros_primario++;
    }
    else { // se a categoria não existe, garantidamente não há outros cursos para buscar pelo ultimo_indice
        categorias_secundario_index novo_index_cat_sec;
        strcpy(novo_index_cat_sec.chave_secundaria, strupr(chave_secundaria));
        novo_index_cat_sec.primeiro_indice = t->qtd_registros_primario;

        t->categorias_secundario_idx[t->qtd_registros_secundario] = novo_index_cat_sec;
        t->qtd_registros_secundario++;

        categorias_primario_index novo_index_cat_pri;
        strcpy(novo_index_cat_pri.chave_primaria, chave_primaria);
        novo_index_cat_pri.proximo_indice = -1;

        t->categorias_primario_idx[t->qtd_registros_primario] = novo_index_cat_pri;
        t->qtd_registros_primario++;
    }

    qsort(t->categorias_secundario_idx, t->qtd_registros_secundario, sizeof(categorias_secundario_index), qsort_categorias_secundario_idx);
}
 
bool inverted_list_secondary_search(int *result, bool exibir_caminho, char *chave_secundaria, inverted_list *t) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    categorias_secundario_index chave_cat_sec; // chave para busca em categorias_secundario_idx
    strcpy(chave_cat_sec.chave_secundaria, strupr(chave_secundaria));
    
    categorias_secundario_index *categoria_buscada = (categorias_secundario_index*) busca_binaria(&chave_cat_sec, t->categorias_secundario_idx, t->qtd_registros_secundario, sizeof(categorias_secundario_index), qsort_categorias_secundario_idx, exibir_caminho, 0);
    
    if (categoria_buscada) {
        if (result != NULL)
            *result = categoria_buscada->primeiro_indice;
        return true;
    }
    else
        return false;
}
 
int inverted_list_primary_search(char result[][TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX], bool exibir_caminho, int indice, int *indice_final, inverted_list *t) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    int i = 0; // contador

    if (exibir_caminho)
        printf(REGS_PERCORRIDOS);

    while (indice != -1) {
        if (exibir_caminho)
            printf(" %d", indice);

        if (result != NULL) 
            strncpy(result[i], t->categorias_primario_idx[indice].chave_primaria, 8);

        if (indice_final != NULL)
            *indice_final = indice;

        indice = t->categorias_primario_idx[indice].proximo_indice;

        i++;
    }

    if (exibir_caminho)
        printf("\n");

    return i;
}
 

char* strpadright(char *str, char pad, unsigned size) {
    for (unsigned i = strlen(str); i < size; ++i)
        str[i] = pad;
    str[size] = '\0';
    return str;
}
 
char *strupr(char *str) {
    for (char *p = str; *p; ++p)
        *p = toupper(*p);
    return str;
}
 
char *strlower(char *str) {
    for (char *p = str; *p; ++p)
         *p = tolower(*p);
    return str;
}


/* Funções da busca binária */
void* busca_binaria(const void *key, const void *base0, size_t nmemb, size_t size, int (*compar)(const void *, const void *), bool exibir_caminho, int retorno_se_nao_encontrado) {
    /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
    // fontes pesquisadas para completar a implementação
    // https://stackoverflow.com/a/56978777
    // https://www.geeksforgeeks.org/find-first-and-last-positions-of-an-element-in-a-sorted-array/

    const void *p;
    int l = 0;
    int r = nmemb - 1;
    int m;
    int cmp;
    int encontrado = -1;

    if (exibir_caminho)
        printf(REGS_PERCORRIDOS);

    while (l <= r) {
        m = l + (r - l + 1)/2;
        p = base0 + m * size;
        cmp = compar(key, p);
        if (exibir_caminho && (encontrado == -1 || cmp == 0))
            printf(" %d", m);
        if (cmp == 0) {
            encontrado = m;
            r = m - 1;
        }
        else if (cmp > 0)
            l = m + 1;
        else
            r = m - 1;
    }

    if (exibir_caminho)
        printf("\n");

    if (encontrado != -1) {
        p = base0 + encontrado * size;
        return (void*)p;
    }
    else { // chave correta não encontrada
        if (retorno_se_nao_encontrado == 0)
            return NULL;

        p = base0 + m * size;
        cmp = compar(key, p);

        // 3 casos possiveis 
        // - nao existe sucessor/antecessor
        // - m está no lugar certo (sucessor ou antecessor)
        // - m está apontando para um lugar 1 posição a frente/atras do que seria o ideal
            
        if (retorno_se_nao_encontrado == 1) { // CASO SUCESSOR

            if (cmp > 0 && m == nmemb - 1) // nao existe sucessor da chave dada
                return NULL;
            else if (cmp > 0) // m é o indice do predecessor da chave, o proximo seria o correto
                p += size;

            return (void*)p;
        }

        else { // CASO ANTECESSOR

            if (cmp < 0 && m == 0) // nao existe antecessor da chave dada
                return NULL;
            else if (cmp < 0) // m é o indice do sucessor da chave, o anterior seria o correto
                p -= size;

            return (void*)p;
        }
    }
}
