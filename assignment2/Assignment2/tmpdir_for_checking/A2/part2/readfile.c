#include <stdio.h>
int main() {    
    FILE *file = fopen("little_bin_file", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    int integer_value;
    double double_value;
    char char_value;
    float float_value;

    fread(&integer_value, sizeof(int), 1, file);
    fread(&double_value, sizeof(double), 1, file);
    fread(&char_value, sizeof(char), 1, file);
    fread(&float_value, sizeof(float), 1, file);

    fclose(file);
    
    printf("%d\n", integer_value);
    printf("%lf\n", double_value);
    printf("%c\n", char_value);
    printf("%f\n", float_value);

    return 0;
}