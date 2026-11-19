#include "project.h"

int main(void) {
    int choice;

    if (!loadAllData()) {
        printf("\nProgram cannot start safely due to data file corruption or unreadable data.\n");
        printf("Please inspect your data files or restore from backup.\n");
        return 1;
    }

    while (1) {
        printf("\n==================================================\n");
        printf("    MOVIE & LOCAL THEATRE TICKET BOOKING SYSTEM   \n");
        printf("   Empowering Commercial & Regional Entertainment \n");
        printf("==================================================\n");
        printf("Select Role:\n");
        printf("1. Buyer (Discover Shows & Book Tickets)\n");
        printf("2. Artist (Register & Manage Regional Events)\n");
        printf("3. Nodal Officer (System Oversight & Reports)\n");
        printf("4. Exit Program\n");
        printf("--------------------------------------------------\n");
        printf("Enter choice (1-4): ");

        if (scanf("%d", &choice) != 1) {
            printf("\nInvalid input! Exiting program...\n");
            saveAllData();
            break;
        }

        switch (choice) {
            case 1:
                buyerPortal();
                break;
            case 2:
                artistPortal();
                break;
            case 3:
                nodalOfficerPortal();
                break;
            case 4:
                saveAllData();
                printf("\nThank you for supporting both cinema and local regional arts. Goodbye!\n");
                exit(0);
            default:
                printf("\nInvalid choice! Please choose between 1 and 4.\n");
        }
    }

    saveAllData();
    return 0;
}
