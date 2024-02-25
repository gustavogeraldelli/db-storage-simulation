testes = 1 2 3 4 5 6 7 8 9 10

all: exec test

test: exec
	@ echo "Criando diretório para saídas..."
	@ mkdir -p saidas

	@ echo "Testando..."
	@ echo "Sua saída: <	Output: >"
	@ $(foreach i, $(testes), ./exec < in/input$(i).txt > saidas/out$(i).txt;)
	@ $(foreach i, $(testes), echo "Teste $(i)" && diff saidas/out$(i).txt out/output$(i).txt;)

exec: T01.c
	@ echo "Compilando o arquivo..."
	@ gcc $< -o $@

clean:
	@ rm -rf saidas
	@ rm -f exec