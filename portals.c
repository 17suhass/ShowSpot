#include "project.h"

void artistRegisterEvent(void) {
    char title[60];
    int category;
    char artist[60];
    char venue[60];
    char time[20];
    float regularPrice;
    float vipPrice;

    printf("\n==================================================\n");
    printf("          REGISTER REGIONAL / LOCAL EVENT         \n");
    printf("==================================================\n");
    printf("Select Event Category:\n");
    printf("2. Regional Theatre / Drama\n");
    printf("3. Local Singing & Music Performance\n");
    printf("4. Traditional & Folk Dance Performance\n");
    printf("5. Other Local Cultural Event\n");
    printf("Enter choice (2-5): ");
    scanf("%d", &category);

    if (category < 2 || category > 5) {
        printf("Invalid category! Regional events must be between 2 and 5.\n");
        return;
    }

    printf("Enter Event Title: ");
    scanf(" %59[^\n]", title);
    printf("Enter Artist / Troupe / Performer Name: ");
    scanf(" %59[^\n]", artist);
    printf("Enter Venue (Hall / Stage / Auditorium): ");
    scanf(" %59[^\n]", venue);
    printf("Enter Schedule / Show Time (e.g. 06:30 PM): ");
    scanf(" %19[^\n]", time);
    printf("Enter Regular Ticket Price (Rows C-J) in Rs.: ");
    scanf("%f", &regularPrice);
    printf("Enter VIP Ticket Price (Rows A-B) in Rs.: ");
    scanf("%f", &vipPrice);

    addEvent(eventIdCounter++, title, category, artist, venue, time, regularPrice, vipPrice, 1);

    printf("\nEvent '%s' registered successfully!\n", title);
    printf("It is now live on the platform for buyers to discover.\n");
    saveAllData();
}

void nodalOfficerOverview(void) {
    struct Event *eventPtr = head;
    struct Booking *bookPtr = bookingHead;
    int totalEvents = 0, commercialEvents = 0, regionalEvents = 0;
    int totalBookings = 0, commercialBookings = 0, regionalBookings = 0;
    float totalRevenue = 0.0, commercialRevenue = 0.0, regionalRevenue = 0.0;
    int demoBookings = 0;
    float demoRevenue = 0.0;

    while (eventPtr != NULL) {
        totalEvents++;
        if (eventPtr->category == 1) {
            commercialEvents++;
        } else {
            regionalEvents++;
        }
        eventPtr = eventPtr->next;
    }

    while (bookPtr != NULL) {
        totalBookings++;
        totalRevenue += bookPtr->price;
        if (bookPtr->eventId == 6) {
            demoBookings++;
            demoRevenue += bookPtr->price;
        }
        if (bookPtr->category == 1) {
            commercialBookings++;
            commercialRevenue += bookPtr->price;
        } else {
            regionalBookings++;
            regionalRevenue += bookPtr->price;
        }
        bookPtr = bookPtr->next;
    }

    printf("\n==============================================================\n");
    printf("             NODAL OFFICER SYSTEM OVERSIGHT REPORT            \n");
    printf("==============================================================\n");
    printf("Total Events Listed              : %d\n", totalEvents);
    printf("  - Commercial Movies            : %d\n", commercialEvents);
    printf("  - Regional & Cultural Events   : %d\n", regionalEvents);
    printf("--------------------------------------------------------------\n");
    printf("Total Tickets Booked             : %d\n", totalBookings);
    printf("  - Commercial Movie Tickets     : %d\n", commercialBookings);
    printf("  - Regional Artist Show Tickets : %d\n", regionalBookings);
    if (demoBookings > 0) {
        printf("  - (Includes %d waitlist demo fixture ticket)\n", demoBookings);
    }
    printf("--------------------------------------------------------------\n");
    printf("Total Revenue Generated          : Rs. %.2f\n", totalRevenue);
    printf("  - Commercial Box Office        : Rs. %.2f\n", commercialRevenue);
    printf("  - Regional Artists Support     : Rs. %.2f\n", regionalRevenue);
    if (demoRevenue > 0.0f) {
        printf("  - (Includes Rs. %.2f waitlist demo fixture revenue)\n", demoRevenue);
    }
    printf("--------------------------------------------------------------\n");
    printf("Note: Event 6 has all seats pre-reserved as a Test Fixture for\n");
    printf("      FIFO waiting-list queue and auto-allocation simulation.\n");
    printf("==============================================================\n");
}

void nodalOfficerAddMovie(void) {
    char title[60];
    char studio[60];
    char venue[60];
    char time[20];
    float regularPrice;
    float vipPrice;

    printf("\n==================================================\n");
    printf("         ADD COMMERCIAL MOVIE SCREENING           \n");
    printf("==================================================\n");
    printf("Enter Movie Title: ");
    scanf(" %59[^\n]", title);
    printf("Enter Production Studio / Distributor: ");
    scanf(" %59[^\n]", studio);
    printf("Enter Multiplex / Cinema Hall Venue: ");
    scanf(" %59[^\n]", venue);
    printf("Enter Show Time (e.g. 09:15 PM): ");
    scanf(" %19[^\n]", time);
    printf("Enter Regular Ticket Price (Rows C-J) in Rs.: ");
    scanf("%f", &regularPrice);
    printf("Enter VIP Ticket Price (Rows A-B) in Rs.: ");
    scanf("%f", &vipPrice);

    addEvent(eventIdCounter++, title, 1, studio, venue, time, regularPrice, vipPrice, 1);
    printf("\nCommercial movie '%s' added successfully!\n", title);
    saveAllData();
}

void searchMenu(void) {
    int choice;
    while (1) {
        printf("\n--------------------------------------------------\n");
        printf("                 SEARCH MENU                      \n");
        printf("--------------------------------------------------\n");
        printf("1. Search by Category (Linear Search)\n");
        printf("2. Search by Artist / Performer Name (Linear Search)\n");
        printf("3. Search Event by Title (Binary Search)\n");
        printf("4. Search Booking by Transaction ID (Linear Search)\n");
        printf("5. Return to Portal\n");
        printf("Enter your choice (1-5): ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            break;
        }

        switch (choice) {
            case 1: {
                int category;
                printf("\nSelect Event Category to Search:\n");
                printf("1. Commercial Movie\n");
                printf("2. Regional Theatre / Drama\n");
                printf("3. Local Singing & Music Performance\n");
                printf("4. Traditional & Folk Dance Performance\n");
                printf("5. Other Local Cultural Event\n");
                printf("Enter category (1-5): ");
                scanf("%d", &category);
                linearSearchEventsByCategory(category);
                break;
            }
            case 2: {
                char artistName[60];
                printf("\nEnter Artist / Performer Name (complete or partial): ");
                scanf(" %59[^\n]", artistName);
                linearSearchEventsByArtist(artistName);
                break;
            }
            case 3: {
                char title[60];
                struct Event *found;
                printf("\nEnter Event Title to Search (Binary Search): ");
                scanf(" %59[^\n]", title);
                found = binarySearchEventByTitle(title);
                if (found != NULL) {
                    printf("\n======================================================\n");
                    printf("               EVENT FOUND (BINARY SEARCH)            \n");
                    printf("======================================================\n");
                    printf("Event ID     : %d\n", found->id);
                    printf("Title        : %s\n", found->title);
                    printf("Category     : %s\n", getCategoryName(found->category));
                    printf("Artist       : %s\n", found->artist);
                    printf("Venue        : %s\n", found->venue);
                    printf("Show Time    : %s\n", found->time);
                    printf("Regular Price: Rs. %.2f\n", found->regularPrice);
                    printf("VIP Price    : Rs. %.2f\n", found->vipPrice);
                    printf("Approval     : %s\n", found->isApproved ? "Approved" : "Pending");
                    printf("======================================================\n");
                } else if (head != NULL) {
                    printf("\nEvent with title '%s' not found.\n", title);
                }
                break;
            }
            case 4: {
                int bookingId;
                printf("\nEnter Booking / Transaction ID to Search: ");
                scanf("%d", &bookingId);
                linearSearchBookingById(bookingId);
                break;
            }
            case 5:
                return;
            default:
                printf("Invalid choice! Choose between 1 and 5.\n");
        }
    }
}

void buyerPortal(void) {
    int choice;
    while (1) {
        printf("\n--------------------------------------------------\n");
        printf("                    BUYER PORTAL                  \n");
        printf("--------------------------------------------------\n");
        printf("1. View All Events & Shows\n");
        printf("2. View Commercial Movies Only\n");
        printf("3. View Regional & Local Theatre Shows Only\n");
        printf("4. Search Events & Bookings\n");
        printf("5. View Seat Layout for an Event\n");
        printf("6. Book a Ticket\n");
        printf("7. Cancel a Ticket\n");
        printf("8. Undo Last Cancellation\n");
        printf("9. Join Waiting List (Fully Booked Events)\n");
        printf("10. View Waiting List for an Event\n");
        printf("11. View My / All Bookings\n");
        printf("12. Return to Main Menu\n");
        printf("Enter your choice (1-12): ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            break;
        }

        switch (choice) {
            case 1:
                displayEvents(0);
                break;
            case 2:
                displayEvents(1);
                break;
            case 3:
                displayEvents(2);
                break;
            case 4:
                searchMenu();
                break;
            case 5:
                viewSeatLayout();
                break;
            case 6:
                bookTicket();
                break;
            case 7:
                cancelTicket();
                break;
            case 8:
                undoCancellation();
                break;
            case 9:
                joinWaitingListPortal();
                break;
            case 10:
                viewWaitingListPortal();
                break;
            case 11:
                displayBookings();
                break;
            case 12:
                return;
            default:
                printf("Invalid choice! Choose between 1 and 12.\n");
        }
    }
}

void artistPortal(void) {
    int choice;
    while (1) {
        printf("\n--------------------------------------------------\n");
        printf("            ARTIST & LOCAL THEATRE PORTAL         \n");
        printf("--------------------------------------------------\n");
        printf("1. Register a Regional / Local Cultural Event\n");
        printf("2. View All Regional & Cultural Shows\n");
        printf("3. Return to Main Menu\n");
        printf("Enter your choice (1-3): ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            break;
        }

        switch (choice) {
            case 1:
                artistRegisterEvent();
                break;
            case 2:
                displayEvents(2);
                break;
            case 3:
                return;
            default:
                printf("Invalid choice! Choose between 1 and 3.\n");
        }
    }
}

void nodalOfficerPortal(void) {
    int choice;
    while (1) {
        printf("\n--------------------------------------------------\n");
        printf("               NODAL OFFICER PORTAL               \n");
        printf("--------------------------------------------------\n");
        printf("1. View Platform Oversight & Revenue Summary\n");
        printf("2. View All Active Bookings (Audit Log)\n");
        printf("3. Search Events & Bookings\n");
        printf("4. View Waiting List for an Event\n");
        printf("5. Add a Commercial Movie Screening\n");
        printf("6. View All Events (Commercial + Regional)\n");
        printf("7. Undo Last Cancellation (Admin Override)\n");
        printf("8. Return to Main Menu\n");
        printf("Enter your choice (1-8): ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            break;
        }

        switch (choice) {
            case 1:
                nodalOfficerOverview();
                break;
            case 2:
                displayBookings();
                break;
            case 3:
                searchMenu();
                break;
            case 4:
                viewWaitingListPortal();
                break;
            case 5:
                nodalOfficerAddMovie();
                break;
            case 6:
                displayEvents(0);
                break;
            case 7:
                undoCancellation();
                break;
            case 8:
                return;
            default:
                printf("Invalid choice! Choose between 1 and 8.\n");
        }
    }
}
