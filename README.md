# 🎓 Sistema de Gestão Acadêmica em C

Projeto desenvolvido para a disciplina de programação do curso de Engenharia de Computação. Consiste em um sistema de gerenciamento executado via terminal, utilizando a linguagem C e estruturas de dados em memória (`structs`) para simular um ambiente de banco de dados relacional.

▶️ **[Assista à demonstração do sistema no YouTube](https://youtu.be/ycaqaLASZlU?si=bh8lzvnWicfuDk8A)**

## 🚀 Funcionalidades

O sistema implementa o ciclo completo de operações **CRUD** e garante a consistência dos dados através de regras estritas:

- **Cadastros (Create):** Inserção de Alunos, Cursos, Turmas e Matrículas.
- **Consultas (Read):** Buscas lineares por IDs e CPFs.
- **Atualizações (Update):** Alteração de registros mantendo as mesmas travas de segurança do cadastro.
- **Exclusão Lógica/Física (Delete):** Implementação de **Integridade Referencial**, bloqueando a remoção de entidades que possuam vínculos (ex: um curso não pode ser apagado se houver matrículas ativas nele).
- **Validação de Dados:** Funções exclusivas para blindar o sistema contra entradas inválidas (Validação de formato de CPF, Datas `DD/MM/AAAA` e campos vazios).
- **Ordenação:** Algoritmo *Bubble Sort* para listagem alfabética de alunos.
- **Consulta Cruzada:** Leitura de Chaves Estrangeiras (FK) para exibir dados em texto plano nos relatórios em vez de apenas IDs numéricos.

## 🛠️ Tecnologias e Estruturas Utilizadas

- **Linguagem C** pura.
- Vetores de `structs` para persistência em tempo de execução.
- Laços de repetição e menus modulares via terminal.

## ⚙️ Como compilar e executar

Para rodar o projeto, você precisará do compilador GCC instalado no seu sistema. Com o terminal aberto na pasta do arquivo, siga os passos:

1. Compile o código-fonte:
```bash
gcc sistema_gestao.c -o sistema_gestao
```

Execute o programa:

```bash
.\sistema_gestao.exe
```
