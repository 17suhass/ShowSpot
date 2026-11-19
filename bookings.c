#include "project.h"

struct Booking *bookingHead = NULL;
int bookingCounter = 101;

void bookTicket(void) {
    int eventId;
    char rowChar;
    int col;
    int rowIndex, colIndex;
    float ticketPrice;
    char buyerName[50];
    struct Event *ptr = head;
    struct Booking *newnode;

    displayEvents(0);
    if (head == NULL) {
        return;
    }

    printf("\nEnter Event/Movie ID you want to book: ");
    scanf("%d", &eventId);

    while (ptr != NULL && ptr->id != eventId) {
        ptr = ptr->next;
    }

    if (ptr == NULL || ptr->isApproved == 0) {
        printf("\nInvalid Event ID or event is not approved yet!\n");
        return;
    }

    if (!hasAvailableSeats(ptr)) {
        int waitChoice;
        printf("\nAll seats for this event are fully booked!\n");
        printf("Would you like to join the Waiting List Queue? (1: Yes, 2: No): ");
        scanf("%d", &waitChoice);
        if (waitChoice == 1) {
            printf("Enter Buyer/Attendee Name: ");
            scanf(" %49[^\n]", buyerName);
            addToWaitingList(ptr, buyerName);
        }
        return;
    }

    displaySeats(ptr);

    printf("\nEnter Row letter (A - J): ");
    scanf(" %c", &rowChar);

    if (rowChar >= 'a' && rowChar <= 'j') {
        rowChar = rowChar - 'a' + 'A';
    }

    if (rowChar < 'A' || rowChar > 'J') {
        printf("\nInvalid row letter! Please select a row between A and J.\n");
        return;
    }

    printf("Enter Seat number (1 - 12): ");
    scanf("%d", &col);

    if (col < 1 || col > 12) {
        printf("\nInvalid seat number! Seat number must be between 1 and 12.\n");
        return;
    }

    rowIndex = rowChar - 'A';
    colIndex = col - 1;

    if (ptr->seats[rowIndex][colIndex] == 'X') {
        printf("\nSorry, seat %c%d is already booked! Please select another available seat.\n", rowChar, col);
        return;
    }

    if (rowIndex < 2) {
        ticketPrice = ptr->vipPrice;
    } else {
        ticketPrice = ptr->regularPrice;
    }

    printf("Enter Buyer/Attendee Name: ");
    scanf(" %49[^\n]", buyerName);

    newnode = (struct Booking *)malloc(sizeof(struct Booking));
    if (newnode == NULL) {
        printf("\nMemory allocation failed for booking!\n");
        return;
    }

    ptr->seats[rowIndex][colIndex] = 'X';

    newnode->bookingId = bookingCounter++;
    strcpy(newnode->buyerName, buyerName);
    newnode->eventId = ptr->id;
    strcpy(newnode->eventTitle, ptr->title);
    newnode->category = ptr->category;
    newnode->row = rowChar;
    newnode->col = col;
    newnode->price = ticketPrice;
    newnode->next = NULL;

    if (bookingHead == NULL) {
        bookingHead = newnode;
    } else {
        struct Booking *temp = bookingHead;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newnode;
    }

    printf("\n======================================================\n");
    printf("               TICKET BOOKED SUCCESSFULLY!            \n");
    printf("======================================================\n");
    printf("Booking ID   : %d\n", newnode->bookingId);
    printf("Buyer Name   : %s\n", newnode->buyerName);
    printf("Event Title  : %s\n", newnode->eventTitle);
    printf("Category     : %s\n", getCategoryName(newnode->category));
    printf("Venue        : %s\n", ptr->venue);
    printf("Show Time    : %s\n", ptr->time);
    printf("Seat Number  : %c%d (%s)\n", newnode->row, newnode->col, (rowIndex < 2) ? "VIP" : "Regular");
    printf("Total Price  : Rs. %.2f\n", newnode->price);
    if (newnode->category >= 2) {
        printf("Note         : Thank you for supporting regional artists!\n");
    }
    printf("======================================================\n");
    saveAllData();
}

void cancelTicket(void) {
    int id;
    int rowIndex, colIndex;
    struct Booking *ptr = bookingHead;
    struct Booking *prev = NULL;
    struct Event *eventPtr = head;

    if (bookingHead == NULL) {
        printf("\nNo bookings found in the system.\n");
        return;
    }

    printf("\nEnter Booking ID to cancel: ");
    scanf("%d", &id);

    while (ptr != NULL && ptr->bookingId != id) {
        prev = ptr;
        ptr = ptr->next;
    }

    if (ptr == NULL) {
        printf("\nBooking ID %d not found.\n", id);
        return;
    }

    while (eventPtr != NULL && eventPtr->id != ptr->eventId) {
        eventPtr = eventPtr->next;
    }

    if (eventPtr == NULL) {
        printf("\nError: Event ID %d associated with this booking does not exist! Cancellation aborted.\n", ptr->eventId);
        return;
    }

    rowIndex = ptr->row - 'A';
    colIndex = ptr->col - 1;

    if (rowIndex < 0 || rowIndex >= 10 || colIndex < 0 || colIndex >= 12) {
        printf("\nError: Invalid seat row or column associated with this booking! Cancellation aborted.\n");
        return;
    }

    if (eventPtr->waitlistFront != NULL) {
        struct Booking *autoBooking = (struct Booking *)malloc(sizeof(struct Booking));
        struct WaitlistNode *qTemp;
        float autoPrice;

        if (autoBooking == NULL) {
            printf("\nMemory allocation failed for waitlist auto-booking!\n");
            return;
        }

        if (rowIndex < 2) {
            autoPrice = eventPtr->vipPrice;
        } else {
            autoPrice = eventPtr->regularPrice;
        }

        autoBooking->bookingId = bookingCounter++;
        strcpy(autoBooking->buyerName, eventPtr->waitlistFront->buyerName);
        autoBooking->eventId = eventPtr->id;
        strcpy(autoBooking->eventTitle, eventPtr->title);
        autoBooking->category = eventPtr->category;
        autoBooking->row = ptr->row;
        autoBooking->col = ptr->col;
        autoBooking->price = autoPrice;
        autoBooking->next = NULL;

        if (!pushCancellation(ptr, 1, autoBooking->bookingId, eventPtr->waitlistFront->buyerName, eventPtr->waitlistFront->token)) {
            free(autoBooking);
            printf("\nCancellation aborted: Unable to save cancellation to undo stack.\n");
            return;
        }

        if (bookingHead == NULL) {
            bookingHead = autoBooking;
        } else {
            struct Booking *bTemp = bookingHead;
            while (bTemp->next != NULL) {
                bTemp = bTemp->next;
            }
            bTemp->next = autoBooking;
        }

        qTemp = eventPtr->waitlistFront;
        eventPtr->waitlistFront = eventPtr->waitlistFront->next;
        if (eventPtr->waitlistFront == NULL) {
            eventPtr->waitlistRear = NULL;
        }

        printf("\nBooking ID %d for '%s' cancelled successfully.\n", ptr->bookingId, ptr->buyerName);
        printf("\n======================================================\n");
        printf("       AUTOMATIC WAITING LIST TICKET ALLOCATION       \n");
        printf("======================================================\n");
        printf("Waitlist Token : #%d\n", qTemp->token);
        printf("Buyer Name     : %s\n", qTemp->buyerName);
        printf("Event Title    : %s\n", eventPtr->title);
        printf("New Booking ID : %d\n", autoBooking->bookingId);
        printf("Seat Number    : %c%d (%s)\n", autoBooking->row, autoBooking->col, (rowIndex < 2) ? "VIP" : "Regular");
        printf("Ticket Price   : Rs. %.2f\n", autoBooking->price);
        printf("Status         : Automatically allocated from FIFO waiting list!\n");
        printf("======================================================\n");

        free(qTemp);
    } else {
        if (!pushCancellation(ptr, 0, 0, "", 0)) {
            printf("\nCancellation aborted: Unable to save cancellation to undo stack.\n");
            return;
        }

        if (eventPtr != NULL) {
            if (rowIndex >= 0 && rowIndex < 10 && colIndex >= 0 && colIndex < 12) {
                if (rowIndex < 2) {
                    eventPtr->seats[rowIndex][colIndex] = 'V';
                } else {
                    eventPtr->seats[rowIndex][colIndex] = 'O';
                }
            }
        }

        printf("\nBooking ID %d for '%s' cancelled successfully.\n", ptr->bookingId, ptr->buyerName);
        printf("Seat %c%d has been released and restored to available status.\n", ptr->row, ptr->col);
    }

    if (prev == NULL) {
        bookingHead = ptr->next;
    } else {
        prev->next = ptr->next;
    }

    free(ptr);
    saveAllData();
}

void displayBookings(void) {
    struct Booking *ptr = bookingHead;

    if (ptr == NULL) {
        printf("\nNo active bookings found.\n");
        return;
    }

    printf("\n========================================================================================================================\n");
    printf("                                                  ACTIVE BOOKINGS LIST                                                  \n");
    printf("========================================================================================================================\n");
    printf("%-12s %-22s %-32s %-14s %-12s\n", "Booking ID", "Buyer Name", "Event Title", "Seat", "Amount");
    printf("------------------------------------------------------------------------------------------------------------------------\n");

    while (ptr != NULL) {
        char seatStr[20];
        sprintf(seatStr, "%c%d (%s)", ptr->row, ptr->col, (ptr->row == 'A' || ptr->row == 'B') ? "VIP" : "Reg");
        printf("%-12d %-22s %-32s %-14s Rs. %-8.2f\n",
               ptr->bookingId, ptr->buyerName, ptr->eventTitle, seatStr, ptr->price);
        ptr = ptr->next;
    }
    printf("========================================================================================================================\n");
}

void linearSearchBookingById(int bookingId) {
    struct Booking *ptr = bookingHead;

    if (ptr == NULL) {
        printf("\nNo active bookings found in the system.\n");
        return;
    }

    while (ptr != NULL) {
        if (ptr->bookingId == bookingId) {
            printf("\n======================================================\n");
            printf("            BOOKING FOUND (LINEAR SEARCH)             \n");
            printf("======================================================\n");
            printf("Booking ID   : %d\n", ptr->bookingId);
            printf("Buyer Name   : %s\n", ptr->buyerName);
            printf("Event ID     : %d\n", ptr->eventId);
            printf("Event Title  : %s\n", ptr->eventTitle);
            printf("Category     : %s\n", getCategoryName(ptr->category));
            printf("Seat Number  : %c%d (%s)\n", ptr->row, ptr->col, (ptr->row == 'A' || ptr->row == 'B') ? "VIP" : "Regular");
            printf("Total Price  : Rs. %.2f\n", ptr->price);
            printf("======================================================\n");
            return;
        }
        ptr = ptr->next;
    }

    printf("\nBooking ID %d not found in active bookings.\n", bookingId);
}

int saveBookingsToFile(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    struct Booking *ptr = bookingHead;
    struct BookingRecord rec;

    if (fp == NULL) {
        printf("\nError: Cannot open %s for writing!\n", filename);
        return 0;
    }

    while (ptr != NULL) {
        rec.bookingId = ptr->bookingId;
        strcpy(rec.buyerName, ptr->buyerName);
        rec.eventId = ptr->eventId;
        strcpy(rec.eventTitle, ptr->eventTitle);
        rec.category = ptr->category;
        rec.row = ptr->row;
        rec.col = ptr->col;
        rec.price = ptr->price;
        if (fwrite(&rec, sizeof(struct BookingRecord), 1, fp) != 1) {
            printf("\nError writing to %s!\n", filename);
            fclose(fp);
            return 0;
        }
        ptr = ptr->next;
    }

    fclose(fp);
    return 1;
}

int loadBookingsFromFile(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    struct BookingRecord rec;
    struct Booking *last = NULL;
    long fileSize;
    long expectedCount;
    long loadedCount = 0;

    if (fp == NULL) {
        if (errno == ENOENT) {
            return 0;
        }
        printf("\nError: Could not open '%s' due to permission or system error!\n", filename);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    while (bookingHead != NULL) {
        struct Booking *temp = bookingHead;
        bookingHead = bookingHead->next;
        free(temp);
    }
    bookingHead = NULL;

    if (fileSize == 0) {
        fclose(fp);
        return 1;
    }

    if (fileSize % (long)sizeof(struct BookingRecord) != 0) {
        printf("\nError: '%s' is corrupted (invalid file size: %ld bytes)!\n", filename, fileSize);
        fclose(fp);
        return -1;
    }

    expectedCount = fileSize / (long)sizeof(struct BookingRecord);

    while (fread(&rec, sizeof(struct BookingRecord), 1, fp) == 1) {
        struct Booking *newnode = (struct Booking *)malloc(sizeof(struct Booking));
        if (newnode == NULL) {
            printf("\nMemory allocation failed while loading booking!\n");
            fclose(fp);
            while (bookingHead != NULL) {
                struct Booking *temp = bookingHead;
                bookingHead = bookingHead->next;
                free(temp);
            }
            return -1;
        }

        newnode->bookingId = rec.bookingId;
        strcpy(newnode->buyerName, rec.buyerName);
        newnode->eventId = rec.eventId;
        strcpy(newnode->eventTitle, rec.eventTitle);
        newnode->category = rec.category;
        newnode->row = rec.row;
        newnode->col = rec.col;
        newnode->price = rec.price;
        newnode->next = NULL;

        if (bookingHead == NULL) {
            bookingHead = newnode;
        } else {
            last->next = newnode;
        }
        last = newnode;

        if (rec.bookingId >= bookingCounter) {
            bookingCounter = rec.bookingId + 1;
        }
        loadedCount++;
    }

    if (ferror(fp) || loadedCount != expectedCount) {
        printf("\nError: Incomplete read from '%s' (loaded %ld of %ld records)!\n", filename, loadedCount, expectedCount);
        fclose(fp);
        while (bookingHead != NULL) {
            struct Booking *temp = bookingHead;
            bookingHead = bookingHead->next;
            free(temp);
        }
        return -1;
    }

    fclose(fp);
    return 1;
}
