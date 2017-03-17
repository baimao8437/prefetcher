#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    FILE *fp = fopen("time.txt", "r");
    FILE *output = fopen("output.txt", "w");
    output = fopen("output.txt", "a");
    if (!fp) {
        printf("ERROR opening input file orig.txt\n");
        exit(0);
    }
    int total_method = atoi(argv[1]);
    int runtime = atoi(argv[2]);

    double orig_sum_a = 0.0, orig_a = 0.0;
    for (int i = 0; i < total_method; i++) {
        orig_sum_a = 0.0;
        char method[30] = "";
        char tmp[30] = "";
        fscanf(fp, "%s", method);
        for (int k = 0 , pos = 0; k < strlen(method) - 10; k++, pos++) {
            if (method[k] == '_') {
                tmp[pos++] = '\\';
                tmp[pos++] = '\\';
                tmp[pos] = '_';
            } else
                tmp[pos] = method[k];
        }
        for (int j = 0; j < runtime; j++) {
            fscanf(fp, "%lf", &orig_a);
            orig_sum_a += orig_a;
        }
        fprintf(output, "%s %lf\n", tmp , orig_sum_a / (double)runtime);
    }
    fclose(fp);
    fclose(output);
    return 0;
}
