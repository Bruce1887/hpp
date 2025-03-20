#include "parser.h"

Board *read_dat_file(char *filename, int N)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size % sizeof(int8_t) != 0)
    {
        fprintf(stderr, "File size is not a multiple of sizeof(int8_t)\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    int8_t *buffer = (int8_t *)malloc(file_size);
    if (buffer == NULL)
    {
        perror("Error allocating memory");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    size_t read_elements = fread(buffer, sizeof(int8_t), file_size, file);
    if (read_elements != file_size)
    {
        perror("Error reading file");
        free(buffer);
        fclose(file);
        exit(EXIT_FAILURE);
    }
    fclose(file);

    Board *b = (Board *)malloc(sizeof(Board));
    b->base = buffer[0];
    b->side = buffer[1];
    assert(file_size == b->side * b->side + 2);
    assert(b->side % b->base == 0);

    int8_t *cells = buffer + 2;
    b->cells = malloc((file_size - 2) * sizeof(Cell));
    memcpy(b->cells, cells, file_size - 2);
    free(buffer);

    b->num_empty = 0;

    b->empty_mask_size = (b->side * b->side + 8 * sizeof(Mask) - 1) / (8 * sizeof(Mask));
    b->cell_mask_size = (b->side + 8 * sizeof(Mask) - 1) / (8 * sizeof(Mask));
    initialise_empty_mask(b);

    initialise_cell_masks(b);

    printf("file size: %ld\n", file_size);

    return b;
}

void initialise_empty_mask(Board *b)
{
    // initialise the empty mask
    b->empty_mask = (Mask *)calloc(b->empty_mask_size, sizeof(Mask));

    for (int i = 0; i < b->empty_mask_size; i++)
    {
        // initialise all bits to 1 (occupied)
        b->empty_mask[i] = (Mask)~0;
    }
    for (int i = 0; i < b->side * b->side; i++)
    {
        if (IS_EMPTY(b->cells[i]))
        {
            b->num_empty++;
            b->empty_mask[i / 64] &= ~((uint64_t)1 << (i % 64)); // Ensure empty cells are 0
        }
    }
}

// initilises the masks for the rows, columns and boxes
void initialise_cell_masks(Board *b)
{
    assert(b->side % b->base == 0);
    
    b->r_mask = (Mask **)malloc(b->side * sizeof(Mask *));
    b->c_mask = (Mask **)malloc(b->side * sizeof(Mask *));
    b->b_mask = (Mask **)malloc(b->side * sizeof(Mask *));

    for (int i = 0; i < b->side; i++)
    {
        b->r_mask[i] = (Mask *)calloc(b->cell_mask_size, sizeof(Mask));
        b->c_mask[i] = (Mask *)calloc(b->cell_mask_size, sizeof(Mask));
        b->b_mask[i] = (Mask *)calloc(b->cell_mask_size, sizeof(Mask));
        for (int j = 0; j < b->cell_mask_size; j++)
        {
            // Initialise all bits to 0 (vacant)
            b->r_mask[i][j] = (Mask)VACANT;
            b->c_mask[i][j] = (Mask)VACANT;
            b->b_mask[i][j] = (Mask)VACANT;
        }
    }

    int num_occupied_found = 0;
    for (int r = 0; r < b->side; r++)
    {
        for (int c = 0; c < b->side; c++)
        {
            Cell val = get_cell(b, r, c);
            if (IS_EMPTY(val) == false)
            {       
                num_occupied_found++;
                Mask *rm = b->r_mask[r];
                Mask *cm = b->c_mask[c];
                Mask *bm = b->b_mask[get_box_idx(b, r, c)];

                update_mask(rm, val, OCCUPIED);
                update_mask(cm, val, OCCUPIED);
                update_mask(bm, val, OCCUPIED);
            }
        }
    }
    assert(num_occupied_found == b->side * b->side - b->num_empty);
}

void write_board_to_file(Board *b)
{
    char *output_file = "output.dat";

    FILE *file = fopen(output_file, "wb");

    if (file == NULL)
    {
        fprintf(stderr, "Error opening file for writing: %s\n", output_file);
        exit(EXIT_FAILURE);
    }

    int num_cells = b->side * b->side;

    int8_t data[num_cells + 2];
    data[0] = b->base;
    data[1] = b->side;
    mempcpy(data + 2, b->cells, b->side * b->side);

    fwrite(data, sizeof(int8_t), num_cells + 2, file);

    printf("wrote final state to %s\n", output_file);
    fclose(file);
}