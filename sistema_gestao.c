#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <ctype.h>

#define max 50 // limite de cadastros do sistema

// variaveis globais pros menus e os contadores de id
int op = 0, sop = 0;
char resp;
int ida = 0, idc = 0, idt = 0, idm = 0;

// --- structs do sistema ---

struct aluno {
    char pk_cpf[15]; // chave primaria
    char nome[max];
    char admissao[11];
    char dtNasc[11];
    char escolaridade[max];
};

struct curso {
    int pk_codCurso; // pk do curso
    char nome[max];
    int cargaHoraria;
    char modalidade[30];
};

struct turma {
    int pk_codTurma; // pk da turma
    char nomeTurma[max];
    int qtdAlunos;
    char turno[20];
};

struct matricula {
    int pk_ra; // pk
    char fk_cpf[15];     // fk puxando o cpf do aluno
    int fk_codTurma;     // fk da turma
    int fk_codCurso;     // fk do curso
    char turno[20];
};

// vetores que vao servir de banco de dados na memoria msm
struct aluno a, tbaluno[max];
struct curso c, tbcurso[max];
struct turma t, tbturma[max];
struct matricula m, tbmatricula[max];

// --- funcoes de validacao ---

// checa se o campo ta vazio (se o cara so deu enter sem querer)
int campoVazio(const char *str) {
    return (strlen(str) == 0);
}

// funcao p/ validar o cpf no formato 000.000.000-00 
int validarCPF(const char *cpf) {
    int i;
    if (strlen(cpf) != 14) return 0;
    for (i = 0; i < 14; i++) {
        if (i == 3 || i == 7) {
            if (cpf[i] != '.') return 0;
        } else if (i == 11) {
            if (cpf[i] != '-') return 0;
        } else {
            if (!isdigit(cpf[i])) return 0;
        }
    }
    return 1;
}

// valida data estilo DD/MM/AAAA garantindo as barras no lugar certo
int validarData(const char *data) {
    int i;
    if (strlen(data) != 10) return 0;
    if (data[2] != '/' || data[5] != '/') return 0;
    for (i = 0; i < 10; i++) {
        if (i != 2 && i != 5 && !isdigit(data[i])) return 0;
    }
    return 1;
}

// --- buscas ---
// varre o vetor procurando o id. se n achar retorna -1

int buscarAluno(const char *cpf) {
    int i;
    for (i = 0; i < ida; i++) {
        if (strcmp(tbaluno[i].pk_cpf, cpf) == 0) return i;
    }
    return -1;
}

int buscarCurso(int codigo) {
    int i;
    for (i = 0; i < idc; i++) {
        if (tbcurso[i].pk_codCurso == codigo) return i;
    }
    return -1;
}

int buscarTurma(int codigo) {
    int i;
    for (i = 0; i < idt; i++) {
        if (tbturma[i].pk_codTurma == codigo) return i;
    }
    return -1;
}

int buscarMatricula(int ra) {
    int i;
    for (i = 0; i < idm; i++) {
        if (tbmatricula[i].pk_ra == ra) return i;
    }
    return -1;
}

// --- checagem de integridade ---
// ve se tem matricula presa ao aluno/curso/turma antes de deixar excluir
int temMatriculaVinculada(const char *cpf, int codCurso, int codTurma, int tipo) {
    int i;
    for (i = 0; i < idm; i++) {
        if (tipo == 1 && strcmp(tbmatricula[i].fk_cpf, cpf) == 0) return 1;
        if (tipo == 2 && tbmatricula[i].fk_codCurso == codCurso) return 1;
        if (tipo == 3 && tbmatricula[i].fk_codTurma == codTurma) return 1;
    }
    return 0;
}

// --- menus do sistema ---

void menu() {
    printf("***********************************\n");
    printf("* MENU PRINCIPAL           *\n");
    printf("***********************************\n");
    printf("* 1 - CADASTRAR                   *\n");
    printf("* 2 - CONSULTAR                   *\n");
    printf("* 3 - ALTERAR                     *\n");
    printf("* 4 - EXCLUIR                     *\n");
    printf("* 5 - ORDENAR                     *\n");
    printf("* 6 - IMPRIMIR RELATÓRIOS         *\n");
    printf("* 7 - FINALIZAR                   *\n");
    printf("***********************************\n\n");
}

void submenu(const char *titulo) {
    printf("***********************************\n");
    printf("* %s             *\n", titulo);
    printf("***********************************\n");
    printf("* 1 - ALUNO                       *\n");
    printf("* 2 - CURSO                       *\n");
    printf("* 3 - TURMA                       *\n");
    printf("* 4 - MATRICULA                   *\n");
    printf("* 5 - RETORNAR                    *\n");
    printf("***********************************\n\n");
}

// --- funçoes p/ imprimir as coisas ---

void exibirAluno(int i) {
    printf("\nCPF: %s | Nome: %s | Admissão: %s | Nasc: %s | Escolaridade: %s", 
           tbaluno[i].pk_cpf, tbaluno[i].nome, tbaluno[i].admissao, tbaluno[i].dtNasc, tbaluno[i].escolaridade);
}

void exibirCurso(int i) {
    printf("\nCódigo: %d | Nome: %s | Carga Horária: %d | Modalidade: %s", 
           tbcurso[i].pk_codCurso, tbcurso[i].nome, tbcurso[i].cargaHoraria, tbcurso[i].modalidade);
}

void exibirTurma(int i) {
    printf("\nCódigo: %d | Nome: %s | Qtd Alunos: %d | Turno: %s", 
           tbturma[i].pk_codTurma, tbturma[i].nomeTurma, tbturma[i].qtdAlunos, tbturma[i].turno);
}

// bonus da av3: consulta cruzada pegando os nomes nas outras structs (usando a FK)
void exibirMatricula(int i) {
    int idxAluno = buscarAluno(tbmatricula[i].fk_cpf);
    int idxCurso = buscarCurso(tbmatricula[i].fk_codCurso);
    int idxTurma = buscarTurma(tbmatricula[i].fk_codTurma);

    char nomeAluno[max] = "N/A";
    char nomeCurso[max] = "N/A";
    char nomeTurma[max] = "N/A";

    if(idxAluno != -1) strcpy(nomeAluno, tbaluno[idxAluno].nome);
    if(idxCurso != -1) strcpy(nomeCurso, tbcurso[idxCurso].nome);
    if(idxTurma != -1) strcpy(nomeTurma, tbturma[idxTurma].nomeTurma);

    printf("\nRA: %d | Aluno: %s | Curso: %s | Turma: %s | Turno: %s", 
           tbmatricula[i].pk_ra, nomeAluno, nomeCurso, nomeTurma, tbmatricula[i].turno);
}

// limpa o lixo q fica no teclado p nao bugar os fgets
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// --- int main ---

int main() {
    setlocale(LC_ALL, "");
    char buscaCpf[15];
    int buscaId, idx, i, j;

    do {
        system("clear || cls");
        menu();
        printf("Escolha uma opção: ");
        if (scanf("%d", &op) != 1) { limparBuffer(); continue; }

        switch (op) {
            case 1: // CADASTRAR
                do {
                    system("clear || cls");
                    submenu("CADASTRAR");
                    printf("Escolha o que cadastrar: ");
                    if (scanf("%d", &sop) != 1) { limparBuffer(); continue; }

                    if (sop == 5) break;

                    switch (sop) {
                        case 1: // cadastra aluno chamando as validacoes
                            if (ida >= max) { printf("\nLimite máximo de alunos atingido!\n"); sleep(2); break; }
                            do {
                                printf("\nCPF (000.000.000-00): ");
                                scanf("%s", a.pk_cpf);
                                if (!validarCPF(a.pk_cpf)) { printf("CPF Inválido! Tente novamente.\n"); continue; }
                                if (buscarAluno(a.pk_cpf) != -1) { printf("CPF já cadastrado!\n"); continue; }
                                break;
                            } while(1);

                            limparBuffer();
                            do {
                                printf("Nome: ");
                                fgets(a.nome, sizeof(a.nome), stdin);
                                a.nome[strcspn(a.nome, "\n")] = '\0';
                            } while(campoVazio(a.nome));

                            do {
                                printf("Admissão (DD/MM/AAAA): ");
                                scanf("%s", a.admissao);
                            } while(!validarData(a.admissao));

                            do {
                                printf("Nascimento (DD/MM/AAAA): ");
                                scanf("%s", a.dtNasc);
                            } while(!validarData(a.dtNasc));

                            limparBuffer();
                            do {
                                printf("Escolaridade: ");
                                fgets(a.escolaridade, sizeof(a.escolaridade), stdin);
                                a.escolaridade[strcspn(a.escolaridade, "\n")] = '\0';
                            } while(campoVazio(a.escolaridade));

                            tbaluno[ida++] = a;
                            printf("\nAluno cadastrado com sucesso!");
                            break;

                        case 2: // cadastra curso
                            if (idc >= max) { printf("\nLimite máximo de cursos atingido!\n"); sleep(2); break; }
                            printf("\nCódigo do Curso: ");
                            scanf("%d", &c.pk_codCurso);
                            if (buscarCurso(c.pk_codCurso) != -1) { printf("Código já cadastrado!\n"); sleep(2); break; }

                            limparBuffer();
                            do {
                                printf("Nome do Curso: ");
                                fgets(c.nome, sizeof(c.nome), stdin);
                                c.nome[strcspn(c.nome, "\n")] = '\0';
                            } while(campoVazio(c.nome));

                            printf("Carga Horária: ");
                            scanf("%d", &c.cargaHoraria);

                            limparBuffer();
                            do {
                                printf("Modalidade (Presencial/EAD): ");
                                fgets(c.modalidade, sizeof(c.modalidade), stdin);
                                c.modalidade[strcspn(c.modalidade, "\n")] = '\0';
                            } while(campoVazio(c.modalidade));

                            tbcurso[idc++] = c;
                            printf("\nCurso cadastrado com sucesso!");
                            break;

                        case 3: // cadastra turma
                            if (idt >= max) { printf("\nLimite máximo de turmas atingido!\n"); sleep(2); break; }
                            printf("\nCódigo da Turma: ");
                            scanf("%d", &t.pk_codTurma);
                            if (buscarTurma(t.pk_codTurma) != -1) { printf("Código já cadastrado!\n"); sleep(2); break; }

                            limparBuffer();
                            do {
                                printf("Nome da Turma: ");
                                fgets(t.nomeTurma, sizeof(t.nomeTurma), stdin);
                                t.nomeTurma[strcspn(t.nomeTurma, "\n")] = '\0';
                            } while(campoVazio(t.nomeTurma));

                            printf("Quantidade Máxima de Alunos: ");
                            scanf("%d", &t.qtdAlunos);

                            limparBuffer();
                            do {
                                printf("Turno: ");
                                fgets(t.turno, sizeof(t.turno), stdin);
                                t.turno[strcspn(t.turno, "\n")] = '\0';
                            } while(campoVazio(t.turno));

                            tbturma[idt++] = t;
                            printf("\nTurma cadastrada com sucesso!");
                            break;

                        case 4: // faz a matricula (tabela associativa)
                            if (idm >= max) { printf("\nLimite máximo de matrículas atingido!\n"); sleep(2); break; }
                            printf("\nRA da Matrícula: ");
                            scanf("%d", &m.pk_ra);
                            if (buscarMatricula(m.pk_ra) != -1) { printf("RA já cadastrado!\n"); sleep(2); break; }

                            printf("CPF do Aluno: ");
                            scanf("%s", m.fk_cpf);
                            if (buscarAluno(m.fk_cpf) == -1) { printf("Aluno não cadastrado! Cadastre o aluno primeiro.\n"); sleep(2); break; }

                            printf("Código da Turma: ");
                            scanf("%d", &m.fk_codTurma);
                            if (buscarTurma(m.fk_codTurma) == -1) { printf("Turma não encontrada!\n"); sleep(2); break; }

                            printf("Código do Curso: ");
                            scanf("%d", &m.fk_codCurso);
                            if (buscarCurso(m.fk_codCurso) == -1) { printf("Curso não encontrado!\n"); sleep(2); break; }

                            printf("Turno: ");
                            scanf("%s", m.turno);

                            tbmatricula[idm++] = m;
                            printf("\nMatrícula realizada com sucesso!");
                            break;
                    }
                    printf("\nDeseja realizar outra operação neste menu? [s/n]: ");
                    scanf(" %c", &resp);
                } while (tolower(resp) == 's');
                break;

            case 2: // CONSULTAR
                system("clear || cls");
                submenu("CONSULTAR");
                printf("Escolha o tipo de consulta: ");
                scanf("%d", &sop);
                if (sop == 5) break;

                // faz a pesquisa dependendo doq o usuario escolheu
                if (sop == 1) {
                    printf("\nDigite o CPF do Aluno: ");
                    scanf("%s", buscaCpf);
                    idx = buscarAluno(buscaCpf);
                    if (idx != -1) exibirAluno(idx); else printf("Aluno não encontrado.");
                } else if (sop == 2) {
                    printf("\nDigite o Código do Curso: ");
                    scanf("%d", &buscaId);
                    idx = buscarCurso(buscaId);
                    if (idx != -1) exibirCurso(idx); else printf("Curso não encontrado.");
                } else if (sop == 3) {
                    printf("\nDigite o Código da Turma: ");
                    scanf("%d", &buscaId);
                    idx = buscarTurma(buscaId);
                    if (idx != -1) exibirTurma(idx); else printf("Turma não encontrada.");
                } else if (sop == 4) {
                    printf("\nDigite o RA: ");
                    scanf("%d", &buscaId);
                    idx = buscarMatricula(buscaId);
                    if (idx != -1) exibirMatricula(idx); else printf("Matrícula não encontrada.");
                }
                limparBuffer(); getchar();
                break;

            case 3: // ALTERAR
                system("clear || cls");
                submenu("ALTERAR");
                printf("Escolha o que deseja alterar: ");
                scanf("%d", &sop);
                if (sop == 5) break;

                if (sop == 1) {
                    printf("\nDigite o CPF do aluno a alterar: ");
                    scanf("%s", buscaCpf);
                    idx = buscarAluno(buscaCpf);
                    if (idx != -1) {
                        limparBuffer();
                        // usando as msm validacoes do cadastro p/ nao dar ruim
                        do {
                            printf("Novo Nome: "); 
                            fgets(tbaluno[idx].nome, max, stdin); 
                            tbaluno[idx].nome[strcspn(tbaluno[idx].nome, "\n")] = '\0';
                        } while(campoVazio(tbaluno[idx].nome));
                        
                        do {
                            printf("Nova Admissão (DD/MM/AAAA): "); 
                            scanf("%s", tbaluno[idx].admissao);
                        } while(!validarData(tbaluno[idx].admissao));
                        
                        do {
                            printf("Nova Data Nasc (DD/MM/AAAA): "); 
                            scanf("%s", tbaluno[idx].dtNasc);
                        } while(!validarData(tbaluno[idx].dtNasc));
                        
                        limparBuffer();
                        do {
                            printf("Nova Escolaridade: "); 
                            fgets(tbaluno[idx].escolaridade, max, stdin); 
                            tbaluno[idx].escolaridade[strcspn(tbaluno[idx].escolaridade, "\n")] = '\0';
                        } while(campoVazio(tbaluno[idx].escolaridade));
                        
                        printf("\nDados alterados com sucesso!");
                    } else printf("Aluno não encontrado.");
                } else if (sop == 2) {
                    printf("\nDigite o código do curso a alterar: ");
                    scanf("%d", &buscaId);
                    idx = buscarCurso(buscaId);
                    if (idx != -1) {
                        limparBuffer();
                        do {
                            printf("Novo Nome do Curso: "); 
                            fgets(tbcurso[idx].nome, max, stdin); 
                            tbcurso[idx].nome[strcspn(tbcurso[idx].nome, "\n")] = '\0';
                        } while(campoVazio(tbcurso[idx].nome));
                        
                        printf("Nova Carga Horária: "); 
                        scanf("%d", &tbcurso[idx].cargaHoraria);
                        
                        limparBuffer();
                        do {
                            printf("Nova Modalidade (Presencial/EAD): "); 
                            fgets(tbcurso[idx].modalidade, 30, stdin); 
                            tbcurso[idx].modalidade[strcspn(tbcurso[idx].modalidade, "\n")] = '\0';
                        } while(campoVazio(tbcurso[idx].modalidade));
                        
                        printf("\nDados alterados com sucesso!");
                    } else printf("Curso não encontrado.");
                } else if (sop == 3) {
                    printf("\nDigite o código da turma a alterar: ");
                    scanf("%d", &buscaId);
                    idx = buscarTurma(buscaId);
                    if (idx != -1) {
                        limparBuffer();
                        do {
                            printf("Novo Nome da Turma: "); 
                            fgets(tbturma[idx].nomeTurma, max, stdin); 
                            tbturma[idx].nomeTurma[strcspn(tbturma[idx].nomeTurma, "\n")] = '\0';
                        } while(campoVazio(tbturma[idx].nomeTurma));

                        printf("Nova Qtd Alunos: "); 
                        scanf("%d", &tbturma[idx].qtdAlunos);
                        
                        limparBuffer();
                        do {
                            printf("Novo Turno: "); 
                            fgets(tbturma[idx].turno, 20, stdin); 
                            tbturma[idx].turno[strcspn(tbturma[idx].turno, "\n")] = '\0';
                        } while(campoVazio(tbturma[idx].turno));
                        
                        printf("\nDados alterados com sucesso!");
                    } else printf("Turma não encontrada.");
                } else if (sop == 4) {
                    // qnd altera a matricula tem q checar de novo se os ids novos existem msm
                    printf("\nDigite o RA da matrícula a alterar: ");
                    scanf("%d", &buscaId);
                    idx = buscarMatricula(buscaId);
                    if (idx != -1) {
                        char tempCpf[15];
                        int tempTurma, tempCurso;

                        printf("Novo CPF do Aluno: "); 
                        scanf("%s", tempCpf);
                        if(buscarAluno(tempCpf) == -1) { printf("Erro: Aluno não cadastrado!\n"); sleep(2); break; }

                        printf("Novo Cód Turma: "); 
                        scanf("%d", &tempTurma);
                        if(buscarTurma(tempTurma) == -1) { printf("Erro: Turma não encontrada!\n"); sleep(2); break; }

                        printf("Novo Cód Curso: "); 
                        scanf("%d", &tempCurso);
                        if(buscarCurso(tempCurso) == -1) { printf("Erro: Curso não encontrado!\n"); sleep(2); break; }

                        strcpy(tbmatricula[idx].fk_cpf, tempCpf);
                        tbmatricula[idx].fk_codTurma = tempTurma;
                        tbmatricula[idx].fk_codCurso = tempCurso;

                        printf("Novo Turno: "); 
                        scanf("%s", tbmatricula[idx].turno);
                        printf("\nMatrícula alterada com sucesso!");
                    } else printf("Matrícula não encontrada.");
                }
                sleep(2);
                break;

            case 4: // EXCLUIR
                system("clear || cls");
                submenu("EXCLUIR");
                printf("Escolha o que deseja excluir: ");
                scanf("%d", &sop);
                if (sop == 5) break;

                // exclusao checando integridade p n dar b.o dps
                if (sop == 1) {
                    printf("\nDigite o CPF do aluno para excluir: ");
                    scanf("%s", buscaCpf);
                    idx = buscarAluno(buscaCpf);
                    if (idx != -1) {
                        if (temMatriculaVinculada(buscaCpf, 0, 0, 1)) {
                            printf("\nNão é possível excluir! Aluno possui matrícula ativa.");
                        } else {
                            // puxa a fila pra tras pra apagar o registro
                            for (i = idx; i < ida - 1; i++) tbaluno[i] = tbaluno[i+1];
                            ida--;
                            printf("\nAluno excluído com sucesso.");
                        }
                    } else printf("Aluno não encontrado.");
                } else if (sop == 2) {
                    printf("\nDigite o código do curso para excluir: ");
                    scanf("%d", &buscaId);
                    idx = buscarCurso(buscaId);
                    if (idx != -1) {
                        if (temMatriculaVinculada("", buscaId, 0, 2)) {
                            printf("\nNão é possível excluir! Existem alunos matriculados neste curso.");
                        } else {
                            for (i = idx; i < idc - 1; i++) tbcurso[i] = tbcurso[i+1];
                            idc--;
                            printf("\nCurso excluído com sucesso.");
                        }
                    } else printf("Curso não encontrado.");
                } else if (sop == 3) {
                    printf("\nDigite o código da turma para excluir: ");
                    scanf("%d", &buscaId);
                    idx = buscarTurma(buscaId);
                    if (idx != -1) {
                        if (temMatriculaVinculada("", 0, buscaId, 3)) {
                            printf("\nNão é possível excluir! Existem alunos vinculados a esta turma.");
                        } else {
                            for (i = idx; i < idt - 1; i++) tbturma[i] = tbturma[i+1];
                            idt--;
                            printf("\nTurma excluída com sucesso.");
                        }
                    } else printf("Turma não encontrado.");
                } else if (sop == 4) {
                    // matricula nao tem dependente entao eh so apagar direto
                    printf("\nDigite o RA da matrícula para excluir: ");
                    scanf("%d", &buscaId);
                    idx = buscarMatricula(buscaId);
                    if (idx != -1) {
                        for (i = idx; i < idm - 1; i++) tbmatricula[i] = tbmatricula[i+1];
                        idm--;
                        printf("\nMatrícula removida com sucesso.");
                    } else printf("Matrícula não encontrada.");
                }
                sleep(2);
                break;

            case 5: // ORDENAR com bubble sort
                // bota os alunos em ordem alfabetica
                if (ida > 1) {
                    struct aluno temp;
                    for (i = 0; i < ida - 1; i++) {
                        for (j = 0; j < ida - i - 1; j++) {
                            if (strcmp(tbaluno[j].nome, tbaluno[j+1].nome) > 0) {
                                temp = tbaluno[j];
                                tbaluno[j] = tbaluno[j+1];
                                tbaluno[j+1] = temp;
                            }
                        }
                    }
                    printf("\nAlunos ordenados por nome com sucesso!");
                } else {
                    printf("\nRegistros insuficientes para ordenação.");
                }
                sleep(2);
                break;

            case 6: // RELATORIOS
                system("clear || cls");
                printf("***********************************\n");
                printf("* TIPO DE RELATÓRIO        *\n");
                printf("***********************************\n");
                printf("* 1 - RELATÓRIO DE ALUNOS         *\n");
                printf("* 2 - RELATÓRIO DE CURSOS         *\n");
                printf("* 3 - RELATÓRIO DE TURMAS         *\n");
                printf("* 4 - RELATÓRIO DE MATRÍCULAS     *\n");
                printf("* 5 - RELATÓRIO GERAL (TODOS)     *\n");
                printf("* 6 - RETORNAR                    *\n");
                printf("***********************************\n\n");
                printf("Escolha uma opção: ");
                if (scanf("%d", &sop) != 1) { limparBuffer(); continue; }

                if (sop == 6) break;

                system("clear || cls");
                printf("\n===== EMISSÃO DE RELATÓRIO =====\n");

                // imprime separado ou geralzao
                if (sop == 1 || sop == 5) {
                    printf("\n--- ALUNOS CADASTRADOS (%d) ---", ida);
                    for(i=0; i<ida; i++) exibirAluno(i);
                    printf("\n");
                }
                if (sop == 2 || sop == 5) {
                    printf("\n--- CURSOS CADASTRADOS (%d) ---", idc);
                    for(i=0; i<idc; i++) exibirCurso(i);
                    printf("\n");
                }
                if (sop == 3 || sop == 5) {
                    printf("\n--- TURMAS CADASTRADAS (%d) ---", idt);
                    for(i=0; i<idt; i++) exibirTurma(i);
                    printf("\n");
                }
                if (sop == 4 || sop == 5) {
                    printf("\n--- MATRÍCULAS ATIVAS (%d) ---", idm);
                    for(i=0; i<idm; i++) exibirMatricula(i);
                    printf("\n");
                }

                printf("\nPressione ENTER para voltar ao menu.");
                limparBuffer(); getchar();
                break;
        }
    } while (op != 7); // sai do loop principal

    system("clear || cls");
    printf("Sistema finalizado com sucesso!\n");
    return 0;
}