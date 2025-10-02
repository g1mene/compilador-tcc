# Compilador de VHDL para SAT de Equivalência de Circuitos Lógicos

## Descrição
Este projeto implementa um compilador que traduz descrições de circuitos em VHDL para fórmulas SAT (Boolean Satisfiability), com o objetivo de verificar a equivalência entre circuitos lógicos. O sistema permite comparar duas implementações de circuitos e determinar se são funcionalmente equivalentes.

## Metodologia SAT para Equivalência

O processo de verificação de equivalência segue os seguintes passos:

1. **Parsing do VHDL**: Reconhecimento de expressões lógicas em VHDL usando Flex e Bison
2. **Construção da AST**: Representação interna das expressões lógicas dos circuitos
3. **Tradução para fórmulas booleanas**: Converter a AST em expressões lógicas padronizadas
4. **Transformação para CNF**: Preparação das fórmulas para forma normal conjuntiva (CNF) para uso em SAT solvers
5. **Geração do problema de equivalência**: Criar a fórmula `c1 XOR c2 = 0` para verificar se dois circuitos são equivalentes
6. **Integração com SAT solver**: Envio da fórmula para o solver existente e análise do resultado:
   - **UNSAT** → Circuitos são equivalentes
   - **SAT** → Circuitos são diferentes (contra-exemplo encontrado)

## Estrutura do Projeto
- `lexico-flex.l` - Analisador léxico implementado em Flex
- `parser.y` - Analisador sintático em Bison (a ser implementado)
- `ast.h/ast.c` - Estruturas e funções para AST (a ser implementado)
- `cnf.h/cnf.c` - Conversão para CNF (a ser implementado)
- `sat_interface.h/sat_interface.c` - Interface com SAT solver (a ser implementado)
- `exemplo.vhd` - Arquivo de exemplo em VHDL para testes
- `tests/` - Diretório com casos de teste (a ser criado)

## Funcionalidades Implementadas
- [x] Análise léxica para tokens básicos de VHDL
- [x] Reconhecimento de operadores lógicos (AND, OR, XOR, NOT)
- [x] Tratamento de comentários VHDL
- [x] Reconhecimento de identificadores e números

## Como Compilar e Executar
```bash
# Compilar o analisador léxico
flex lexico-flex.l
gcc lex.yy.c -o compilador

# Executar com arquivo de teste
./compilador < exemplo.vhd
```

## TODO
- [x] **Análise léxica**: Reconhecimento de tokens VHDL com Flex
- [ ] **Parsing VHDL**: Implementar gramática Bison para expressões lógicas
- [ ] **Construção da AST**: Definir estruturas de nós e ações semânticas no Bison para construir árvore sintática
- [ ] **Tradução para fórmulas booleanas**: Converter AST em expressões lógicas padronizadas
- [ ] **Transformação para CNF**: Implementar conversão para forma normal conjuntiva
- [ ] **Geração do problema de equivalência**: Criar fórmula `c1 XOR c2 = 0`
- [ ] **Integração com SAT solver**: Conectar com solver existente (MiniSAT/Z3)
- [ ] **Testes de validação**: Verificar com circuitos de equivalência conhecida

## Autor
- **Nome:** Gabriel Gimenez Vieira
- **Curso:** Engenharia de Computação
- **Universidade:** Universidade Tecnologica Federal do Paraná - UTFPR
- **Orientador:** Prof. Dr. Ricardo T. Olivieira

## Cronograma
- **Fase 1:** Análise léxica e sintática de VHDL
- **Fase 2:** Tradução para representação intermediária
- **Fase 3:** Geração de fórmulas SAT (CNF)
- **Fase 4:** Integração com solver e verificação de equivalência
- **Fase 5:** Testes e validação com circuitos reais

---
*Última atualização: Setembro 2025*