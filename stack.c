#include "project.h"

struct StackNode *top = NULL;

int pushCancellation(struct Booking *ptr, int autoAllocated, int autoBookingId, const char autoBuyerName[], int autoToken) {
    struct StackNode *newnode = (struct StackNode *)malloc(sizeof(struct StackNode));

    if (newnode == NULL) {
        printf("\nMemory allocation failed for cancellation undo stack!\n");
        return 0;
    }

    newnode->bookingId = ptr->bookingId;
    strcpy(newnode->buyerName, ptr->buyerName);
    newnode->eventId = ptr->eventId;
    strcpy(newnode->eventTitle, ptr->eventTitle);
    newnode->category = ptr->category;
    newnode->row = ptr->row;
    newnode->col = ptr->col;
    newnode->price = ptr->price;
    newnode->autoAllocated = autoAllocated;
    newnode->autoBookingId = autoBookingId;
    strcpy(newnode->autoBuyerName, autoBuyerName);
    newnode->autoToken = autoToken;
    newnode->next = top;
    top = newnode;

    return 1;
}

void undoCancellation(void) {
    int rowIndex, colIndex;
    struct StackNode *temp;
    struct Event *eventPtr;
    struct Booking *newnode;

    if (top == NULL) {
        printf("\nNo cancellations available to undo.\n");
        return;
    }

    temp = top;
    rowIndex = temp->row - 'A';
    colIndex = temp->col - 1;

    if (rowIndex < 0 || rowIndex >= 10 || colIndex < 0 || colIndex >= 12) {
        printf("\nError: Corrupted seat data in cancellation record.\n");
        return;
    }

    eventPtr = head;
    while (eventPtr != NULL && eventPtr->id != temp->eventId) {
        eventPtr = eventPtr->next;
    }

    if (eventPtr == NULL) {
        printf("\nError: The event for this cancelled booking no longer exists.\n");
        return;
    }

    if (temp->autoAllocated == 1) {
        struct Booking *bPtr = bookingHead;
        struct Booking *bPrev = NULL;
        struct WaitlistNode *retNode;

        while (bPtr != NULL && bPtr->bookingId != temp->autoBookingId) {
            bPrev = bPtr;
            bPtr = bPtr->next;
        }

        if (bPtr == NULL) {
            printf("\nCannot undo cancellation: Auto-allocated booking state has changed!\n");
            printf("The cancellation record remains safely on the stack.\n");
            return;
        }

        newnode = (struct Booking *)malloc(sizeof(struct Booking));
        if (newnode == NULL) {
            printf("\nMemory allocation failed while restoring booking!\n");
            return;
        }

        retNode = (struct WaitlistNode *)malloc(sizeof(struct WaitlistNode));
        if (retNode == NULL) {
            free(newnode);
            printf("\nMemory allocation failed while returning buyer to waiting list!\n");
            return;
        }

        if (bPrev == NULL) {
            bookingHead = bPtr->next;
        } else {
            bPrev->next = bPtr->next;
        }
        free(bPtr);

        retNode->token = temp->autoToken;
        retNode->eventId = eventPtr->id;
        strcpy(retNode->buyerName, temp->autoBuyerName);
        retNode->next = eventPtr->waitlistFront;
        eventPtr->waitlistFront = retNode;
        if (eventPtr->waitlistRear == NULL) {
            eventPtr->waitlistRear = retNode;
        }

        newnode->bookingId = temp->bookingId;
        strcpy(newnode->buyerName, temp->buyerName);
        newnode->eventId = temp->eventId;
        strcpy(newnode->eventTitle, temp->eventTitle);
        newnode->category = temp->category;
        newnode->row = temp->row;
        newnode->col = temp->col;
        newnode->price = temp->price;
        newnode->next = NULL;

        if (bookingHead == NULL) {
            bookingHead = newnode;
        } else {
            struct Booking *bTemp = bookingHead;
            while (bTemp->next != NULL) {
                bTemp = bTemp->next;
            }
            bTemp->next = newnode;
        }

        eventPtr->seats[rowIndex][colIndex] = 'X';

        top = top->next;

        printf("\n======================================================\n");
        printf("             CANCELLATION UNDONE SUCCESSFULLY!        \n");
        printf("======================================================\n");
        printf("Original Booking Restored:\n");
        printf("  Booking ID   : %d\n", newnode->bookingId);
        printf("  Buyer Name   : %s\n", newnode->buyerName);
        printf("  Seat Number  : %c%d (%s)\n", newnode->row, newnode->col, (rowIndex < 2) ? "VIP" : "Regular");
        printf("Waiting List Update:\n");
        printf("  Buyer '%s' (Token #%d) returned to front of waiting list!\n", retNode->buyerName, retNode->token);
        printf("======================================================\n");

        free(temp);
    } else {
        if (eventPtr->seats[rowIndex][colIndex] == 'X') {
            printf("\nCannot undo cancellation: Seat %c%d is currently occupied by another booking!\n", temp->row, temp->col);
            printf("The cancellation record remains safely on the stack.\n");
            return;
        }

        newnode = (struct Booking *)malloc(sizeof(struct Booking));
        if (newnode == NULL) {
            printf("\nMemory allocation failed while restoring booking!\n");
            return;
        }

        eventPtr->seats[rowIndex][colIndex] = 'X';

        newnode->bookingId = temp->bookingId;
        strcpy(newnode->buyerName, temp->buyerName);
        newnode->eventId = temp->eventId;
        strcpy(newnode->eventTitle, temp->eventTitle);
        newnode->category = temp->category;
        newnode->row = temp->row;
        newnode->col = temp->col;
        newnode->price = temp->price;
        newnode->next = NULL;

        if (bookingHead == NULL) {
            bookingHead = newnode;
        } else {
            struct Booking *bTemp = bookingHead;
            while (bTemp->next != NULL) {
                bTemp = bTemp->next;
            }
            bTemp->next = newnode;
        }

        top = top->next;

        printf("\n======================================================\n");
        printf("             CANCELLATION UNDONE SUCCESSFULLY!        \n");
        printf("======================================================\n");
        printf("Booking ID   : %d (Restored)\n", newnode->bookingId);
        printf("Buyer Name   : %s\n", newnode->buyerName);
        printf("Event Title  : %s\n", newnode->eventTitle);
        printf("Category     : %s\n", getCategoryName(newnode->category));
        printf("Venue        : %s\n", eventPtr->venue);
        printf("Show Time    : %s\n", eventPtr->time);
        printf("Seat Number  : %c%d (%s)\n", newnode->row, newnode->col, (rowIndex < 2) ? "VIP" : "Regular");
        printf("Ticket Price : Rs. %.2f\n", newnode->price);
        printf("Status       : Booking restored and seat marked as booked.\n");
        printf("======================================================\n");

        free(temp);
    }
    saveAllData();
}

int saveCancellationsToFile(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    struct StackNode **arr;
    int count = 0;
    int i;
    struct StackNode *ptr;
    struct StackRecord rec;

    if (fp == NULL) {
        printf("\nError: Cannot open %s for writing!\n", filename);
        return 0;
    }

    ptr = top;
    while (ptr != NULL) {
        count++;
        ptr = ptr->next;
    }

    if (count == 0) {
        fclose(fp);
        return 1;
    }

    arr = (struct StackNode **)malloc(count * sizeof(struct StackNode *));
    if (arr == NULL) {
        printf("\nMemory allocation failed while saving cancellations!\n");
        fclose(fp);
        return 0;
    }

    ptr = top;
    for (i = 0; i < count; i++) {
        arr[i] = ptr;
        ptr = ptr->next;
    }

    for (i = count - 1; i >= 0; i--) {
        rec.bookingId = arr[i]->bookingId;
        strcpy(rec.buyerName, arr[i]->buyerName);
        rec.eventId = arr[i]->eventId;
        strcpy(rec.eventTitle, arr[i]->eventTitle);
        rec.category = arr[i]->category;
        rec.row = arr[i]->row;
        rec.col = arr[i]->col;
        rec.price = arr[i]->price;
        rec.autoAllocated = arr[i]->autoAllocated;
        rec.autoBookingId = arr[i]->autoBookingId;
        strcpy(rec.autoBuyerName, arr[i]->autoBuyerName);
        rec.autoToken = arr[i]->autoToken;
        if (fwrite(&rec, sizeof(struct StackRecord), 1, fp) != 1) {
            printf("\nError writing to %s!\n", filename);
            free(arr);
            fclose(fp);
            return 0;
        }
    }

    free(arr);
    fclose(fp);
    return 1;
}

int loadCancellationsFromFile(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    struct StackRecord rec;
    struct StackNode *newnode;
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

    while (top != NULL) {
        struct StackNode *temp = top;
        top = top->next;
        free(temp);
    }
    top = NULL;

    if (fileSize == 0) {
        fclose(fp);
        return 1;
    }

    if (fileSize % (long)sizeof(struct StackRecord) != 0) {
        printf("\nError: '%s' is corrupted (invalid file size: %ld bytes)!\n", filename, fileSize);
        fclose(fp);
        return -1;
    }

    expectedCount = fileSize / (long)sizeof(struct StackRecord);

    while (fread(&rec, sizeof(struct StackRecord), 1, fp) == 1) {
        newnode = (struct StackNode *)malloc(sizeof(struct StackNode));
        if (newnode == NULL) {
            printf("\nMemory allocation failed while loading cancellations!\n");
            fclose(fp);
            while (top != NULL) {
                struct StackNode *temp = top;
                top = top->next;
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
        newnode->autoAllocated = rec.autoAllocated;
        newnode->autoBookingId = rec.autoBookingId;
        strcpy(newnode->autoBuyerName, rec.autoBuyerName);
        newnode->autoToken = rec.autoToken;
        newnode->next = top;
        top = newnode;

        if (rec.bookingId >= bookingCounter) {
            bookingCounter = rec.bookingId + 1;
        }
        if (rec.autoBookingId >= bookingCounter) {
            bookingCounter = rec.autoBookingId + 1;
        }
        if (rec.autoToken >= waitTokenCounter) {
            waitTokenCounter = rec.autoToken + 1;
        }
        loadedCount++;
    }

    if (ferror(fp) || loadedCount != expectedCount) {
        printf("\nError: Incomplete read from '%s'!\n", filename);
        fclose(fp);
        while (top != NULL) {
            struct StackNode *temp = top;
            top = top->next;
            free(temp);
        }
        return -1;
    }

    fclose(fp);
    return 1;
}
