#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NUM_USUARIOS 600

int GAME_IDS[] = { 
    174430, 224517, 161936, 342942, 233078, 167791, 291457, 187645, 115746, 162886,
    220308, 316554, 182028, 12333, 193738, 169786, 84876, 167355, 173346, 28720,
    124361, 177736, 246900, 120677, 266192, 266507, 205637, 237182, 312484, 164928,
    199792, 183394, 96848, 251247, 175914, 285774, 192135, 324856, 3076, 102794,
    256960, 170216, 31260, 247763, 185343, 221107, 205059, 276025, 184267, 295947,
    341169, 284083, 126163, 2651, 314040, 521, 161533, 216132, 164153, 35677,
    55690, 244521, 209010, 125153, 266810, 284378, 124742, 230802, 255984, 72125,
    191189, 28143, 201808, 182874, 25613, 157354, 200680, 159675, 180263, 229853,
    121921, 171623, 110327, 68448, 62219, 264220, 253344, 236457, 93, 317985,
    122515, 37111, 18602, 231733, 172386, 40834, 170042, 73439, 269385, 146021
};
int TOTAL_GAMES = 100;

int CAT_IDS[] = { 
     1001, 1002, 1008, 1009, 1010, 1011, 1013, 1015, 1016, 1017,
    1019, 1020, 1021, 1022, 1023, 1024, 1026, 1028, 1029, 1032,
    1035, 1036, 1039, 1040, 1044, 1046, 1047, 1050, 1052, 1055,
    1064, 1069, 1070, 1081, 1082, 1084, 1086, 1088, 1089, 1090,
    1093, 1094, 1097, 1101, 1102, 1113, 1115, 1116, 1118, 2145, 2710
};
int TOTAL_CATS = 51;

int MEC_IDS[] = { 
    2001, 2002, 2004, 2005, 2007, 2008, 2009, 2011, 2012, 2013,
    2015, 2016, 2017, 2018, 2019, 2020, 2021, 2023, 2026, 2027,
    2028, 2040, 2041, 2043, 2046, 2047, 2048, 2057, 2070, 2072,
    2078, 2079, 2080, 2081, 2082, 2661, 2664, 2676, 2685, 2686,
    2689, 2813, 2814, 2819, 2820, 2822, 2823, 2824, 2826, 2827,
    2828, 2829, 2830, 2833, 2835, 2838, 2839, 2840, 2843, 2846,
    2847, 2849, 2850, 2851, 2853, 2854, 2856, 2857, 2860, 2864,
    2871, 2874, 2875, 2876, 2884, 2886, 2887, 2888, 2889, 2891,
    2893, 2897, 2900, 2901, 2902, 2903, 2904, 2910, 2911, 2912,
    2914, 2915, 2916, 2919, 2922, 2924, 2926, 2927, 2931, 2933,
    2935, 2939, 2940, 2947, 2948, 2953, 2955, 2957, 2958, 2959,
    2960, 2961, 2967, 2974, 2975, 2978, 2984, 2987, 3001, 3004,
    3005, 3100, 3104
};
int TOTAL_MECS = 123;

const char* NOMES[] = {
    "Ana", "Bruno", "Carlos", "Daniela", "Eduardo", "Fernanda", "Gabriel", "Helena",
    "Iago", "Julia", "Lucas", "Mariana", "Nicolas", "Olivia", "Pietra", "Rafaela",
    "Samuel", "Tatiane", "Vitor", "Yasmin", "Felipe", "Larissa", "Gustavo", "Beatriz",
    "Andressa", "Luana", "Giovany", "Raphael", "Marcelo", "Rodrig", "Pedro", "Clara",
    "Isabela", "Henrique", "Heitor", "Arthur", "Murilo", "Thauan", "Bruno", "Emily"
};
int TOTAL_NOMES = 40;

const char* SOBRENOMES[] = {
    "Silva", "Ferreira", "Oliveira", "Martins", "Lima", "Pereira", "Costa",
    "Rodrigues", "Almeida", "Nascimento", "Alves", "Carvalho", "Mendes", "Ribeiro", "Torres"
};
int TOTAL_SOBRENOMES = 15;

// Função auxiliar para embaralhar array (Fisher-Yates)
// Usada para escolher jogos aleatórios sem repetir para o mesmo usuário
void shuffle(int *array, size_t n) {
    if (n > 1) {
        size_t i;
        for (i = 0; i < n - 1; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

int gerar_nota() {
    int r = rand() % 100;
    if (r < 5) return 1;       // 5% chance de nota 1
    if (r < 15) return 2;      // 10% chance de nota 2
    if (r < 35) return 3;      // 20% chance de nota 3
    if (r < 70) return 4;      // 35% chance de nota 4
    return 5;                  // 30% chance de nota 5
}

int main() {
    // Inicializa semente aleatória
    srand(time(NULL));

    FILE *f = fopen("users.json", "w");
    if (f == NULL) {
        printf("Erro ao criar arquivo users.json!\n");
        return 1;
    }

    fprintf(f, "[\n");

    for (int i = 1; i <= NUM_USUARIOS; i++) {
        // 1. Gera ID e Nome
        fprintf(f, "  {\n");
        fprintf(f, "    \"id\": \"u%d\",\n", i);
        
        const char* nome = NOMES[rand() % TOTAL_NOMES];
        const char* sob = SOBRENOMES[rand() % TOTAL_SOBRENOMES];
        fprintf(f, "    \"name\": \"%s %s\",\n", nome, sob);
        
        // 2. Idade (13 a 80)
        fprintf(f, "    \"age\": %d,\n", 13 + rand() % 67);

        // 3. Categorias Favoritas (1 a 3 categorias)
        fprintf(f, "    \"favorite_categories\": [");
        int num_cats = 1 + rand() % 3;
        // Copia array de categorias para embaralhar
        int temp_cats[51];
        memcpy(temp_cats, CAT_IDS, sizeof(CAT_IDS));
        shuffle(temp_cats, TOTAL_CATS);
        
        for(int c = 0; c < num_cats; c++) {
            fprintf(f, "%d", temp_cats[c]);
            if(c < num_cats - 1) fprintf(f, ", ");
        }
        fprintf(f, "],\n");

        // 6. Mecanicas Favoritas (2 a 4 mecanicas)
        fprintf(f, "    \"favorite_mechanics\": [");
        int num_mecs = 2 + rand() % 4;
        // Copia array de mecanicas para embaralhar
        int temp_mecs[123];
        memcpy(temp_mecs, MEC_IDS, sizeof(MEC_IDS));
        shuffle(temp_mecs, TOTAL_MECS);

        for(int m = 0; m < num_mecs; m++) {
            fprintf(f, "%d", temp_mecs[m]);
            if(m < num_mecs - 1) fprintf(f, ", ");
        }
        fprintf(f, "],\n");

        // 5. Ratings (Avaliações)
        fprintf(f, "    \"ratings\": [\n");
        
        // Copia array de jogos para embaralhar e pegar unicos
        int temp_games[100];
        memcpy(temp_games, GAME_IDS, sizeof(GAME_IDS));
        shuffle(temp_games, TOTAL_GAMES);

        // Cada usuário avalia entre 3 e 12 jogos
        int num_ratings = 3 + rand() % 10;
        
        for(int r = 0; r < num_ratings; r++) {
            int game_id = temp_games[r];
            int stars = gerar_nota();
            
            fprintf(f, "      { \"id\": %d, \"stars\": %d }", game_id, stars);
            
            //vírgula se não for o último rating
            if(r < num_ratings - 1) fprintf(f, ",");
            fprintf(f, "\n");
        }

        fprintf(f, "    ]\n"); // Fecha ratings
        fprintf(f, "  }");     // Fecha objeto usuario

        // Vírgula se não for o último usuário
        if (i < NUM_USUARIOS) fprintf(f, ",");
        fprintf(f, "\n");
    }

    fprintf(f, "]\n"); // Fecha array principal
    fclose(f);

    printf("Sucesso! %d usuarios gerados em 'users1.json'.\n", NUM_USUARIOS);
    return 0;
}