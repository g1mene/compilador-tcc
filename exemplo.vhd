-- Exemplo de código VHDL para teste do parser
-- Comentários são ignorados pelo analisador léxico

-- Atribuições de sinais
signal1 <= 1;
signal2 <= 0;

-- Operações lógicas básicas  
resultado1 <= signal1 AND signal2;
resultado2 <= signal1 OR signal2;
resultado3 <= signal1 XOR signal2;

-- Operação NOT
resultado4 <= NOT signal1;

-- Expressões mais complexas
resultado5 <= (signal1 AND signal2) OR (NOT signal2);
resultado6 <= signal1 XOR (signal2 AND 1);

-- Operações com números
temp1 <= 1 AND 0;
temp2 <= NOT 1;
temp3 <= (1 OR 0) XOR 1;