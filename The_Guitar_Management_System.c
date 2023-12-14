#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct Guitar {
    int serialNumber;
    char model[50];
    int numStrings;
    float scaleLength;
    bool hasPickups;
    bool passedQualityControl;
};

void inputGuitarInfo(struct Guitar *guitar, int guitarNum) {
    printf("\nEnter information for Guitar %d:\n", guitarNum);

    printf("Serial Number: ");
    scanf("%d", &(guitar->serialNumber));

    printf("Model: ");
    scanf("%s", guitar->model);

    printf("Number of Strings: ");
    scanf("%d", &(guitar->numStrings));

    printf("Scale Length (in inches): ");
    scanf("%f", &(guitar->scaleLength));

    printf("Does the guitar have pickups? (1 for Yes, 0 for No): ");
    scanf("%d", &(guitar->hasPickups));

    guitar->passedQualityControl = true;

    if (guitar->numStrings < 6 || guitar->numStrings > 8) {
        printf("Quality control failed: Number of strings should be between 6 and 8.\n");
        guitar->passedQualityControl = false;
    }

    if (guitar->scaleLength < 24.0 || guitar->scaleLength > 26.0) {
        printf("Quality control failed: Scale length should be between 24.0 and 26.0 inches.\n");
        guitar->passedQualityControl = false;
    }
}

void writeGuitarDetails(FILE *file, struct Guitar *guitar) {
    fprintf(file, "Serial Number: %d\n", guitar->serialNumber);
    fprintf(file, "Model: %s\n", guitar->model);
    fprintf(file, "Number of Strings: %d\n", guitar->numStrings);
    fprintf(file, "Scale Length: %.2f inches\n", guitar->scaleLength);
    fprintf(file, "Pickups: %s\n", guitar->hasPickups ? "Yes" : "No");
    fprintf(file, "Quality Control: %s\n", guitar->passedQualityControl ? "Passed" : "Failed");
    fprintf(file, "----------------------------------\n");
}

int addGuitar(struct Guitar **guitars, int *numGuitars, FILE *file) {
    (*numGuitars)++;
    *guitars = realloc(*guitars, (*numGuitars) * sizeof(struct Guitar));
    if (*guitars == NULL) {
        perror("Error reallocating memory");
        return 0;
    }

    inputGuitarInfo(&(*guitars)[*numGuitars - 1], *numGuitars);
    writeGuitarDetails(file, &(*guitars)[*numGuitars - 1]);

    if (ferror(file)) {
        perror("Error writing to file");
        return 0;
    }

    return 1;
}

int searchGuitar(struct Guitar *guitars, int numGuitars, int serialNumber) {
    for (int i = 0; i < numGuitars; i++) {
        if (guitars[i].serialNumber == serialNumber) {
            return i; 
        }
    }
    return -1;  
}

int removeGuitar(struct Guitar **guitars, int *numGuitars, int serialNumber) {
    int index = searchGuitar(*guitars, *numGuitars, serialNumber);
    if (index != -1) {
       
        for (int i = index; i < *numGuitars - 1; i++) {
            (*guitars)[i] = (*guitars)[i + 1];
        }
        (*numGuitars)--;
        *guitars = realloc(*guitars, (*numGuitars) * sizeof(struct Guitar));
        return 1;  
    }
    return 0;  
}

int main() {
    FILE *file = fopen("guitar_quality_report.txt", "w");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    int numGuitars = 0;
    struct Guitar *guitars = NULL;

    int choice;
    do {
        printf("\nMenu:\n");
        printf("1. Add Guitar\n");
        printf("2. Remove Guitar\n");
        printf("3. Search for Guitar\n");
        printf("4. Display Guitars\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }

        while (getchar() != '\n');

        switch (choice) {
            case 1:
                if (!addGuitar(&guitars, &numGuitars, file)) {
                    fclose(file);
                    free(guitars);
                    return 1;
                }
                break;

            case 2: {
                int serialNumberToRemove;
                printf("Enter the serial number of the guitar to remove: ");
                scanf("%d", &serialNumberToRemove);
                while (getchar() != '\n');

                if (removeGuitar(&guitars, &numGuitars, serialNumberToRemove)) {
                    printf("Guitar removed successfully.\n");
                } else {
                    printf("Guitar not found.\n");
                }
                break;
            }

            case 3: {
                int serialNumberToSearch;
                printf("Enter the serial number of the guitar to search: ");
                scanf("%d", &serialNumberToSearch);
                while (getchar() != '\n');

                int index = searchGuitar(guitars, numGuitars, serialNumberToSearch);
                if (index != -1) {
                    printf("Guitar found at index %d.\n", index);
                    
                } else {
                    printf("Guitar not found.\n");
                }
                break;
            }

            case 4:
                printf("\nGuitar Details:\n");
                for (int i = 0; i < numGuitars; i++) {
                    printf("\nDetails for Guitar %d:\n", i + 1);
                    writeGuitarDetails(stdout, &guitars[i]);
                }
                break;

            case 5:
              
                break;

            default:
                printf("Invalid choice. Please enter a number between 1 and 5.\n");
                break;
        }

    } while (choice != 5);

    if (fclose(file) != 0) {
        perror("Error closing file");
        free(guitars);
        return 1;
    }

    free(guitars);
    printf("\nGuitar quality report written to guitar_quality_report.txt\n");

    return 0;
}

