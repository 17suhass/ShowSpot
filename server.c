#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#include "project.h"
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define PORT 8080
#define MAX_REQ 16384
#define MAX_RESP 131072
#define MAX_JSON 131072

static void jsonEscape(const char *src, char *dst, int dstSize) {
    int si = 0, di = 0;
    while (src[si] != '\0' && di < dstSize - 2) {
        if (src[si] == '"') {
            if (di + 2 >= dstSize - 1) break;
            dst[di++] = '\\';
            dst[di++] = '"';
        } else if (src[si] == '\\') {
            if (di + 2 >= dstSize - 1) break;
            dst[di++] = '\\';
            dst[di++] = '\\';
        } else if (src[si] == '\n') {
            if (di + 2 >= dstSize - 1) break;
            dst[di++] = '\\';
            dst[di++] = 'n';
        } else if (src[si] == '\r') {
            if (di + 2 >= dstSize - 1) break;
            dst[di++] = '\\';
            dst[di++] = 'r';
        } else {
            dst[di++] = src[si];
        }
        si++;
    }
    dst[di] = '\0';
}

static void urlDecode(const char *src, char *dst, int dstSize) {
    int si = 0, di = 0;
    while (src[si] != '\0' && di < dstSize - 1) {
        if (src[si] == '+') {
            dst[di++] = ' ';
            si++;
        } else if (src[si] == '%' && src[si+1] != '\0' && src[si+2] != '\0') {
            char hex[3];
            int val;
            hex[0] = src[si+1];
            hex[1] = src[si+2];
            hex[2] = '\0';
            val = (int)strtol(hex, NULL, 16);
            dst[di++] = (char)val;
            si += 3;
        } else {
            dst[di++] = src[si++];
        }
    }
    dst[di] = '\0';
}

static int getQueryParam(const char *query, const char *key, char *out, int outSize) {
    char keyEq[128];
    const char *p;
    int keyLen;

    if (query == NULL || key == NULL || out == NULL || outSize <= 0) return 0;
    out[0] = '\0';
    snprintf(keyEq, sizeof(keyEq), "%s=", key);
    keyLen = (int)strlen(keyEq);

    p = query;
    while (*p != '\0') {
        if (strncmp(p, keyEq, keyLen) == 0) {
            const char *start = p + keyLen;
            const char *end = start;
            int len;
            char tmp[1024];
            while (*end != '\0' && *end != '&') end++;
            len = (int)(end - start);
            if (len >= outSize) len = outSize - 1;
            if (len >= (int)sizeof(tmp)) len = (int)sizeof(tmp) - 1;
            strncpy(tmp, start, len);
            tmp[len] = '\0';
            urlDecode(tmp, out, outSize);
            return 1;
        }
        while (*p != '\0' && *p != '&') p++;
        if (*p == '&') p++;
    }
    return 0;
}

static int getJsonParam(const char *json, const char *key, char *out, int outSize) {
    char keyPattern[128];
    const char *p;
    int i = 0;

    if (json == NULL || key == NULL || out == NULL || outSize <= 0) return 0;
    out[0] = '\0';

    snprintf(keyPattern, sizeof(keyPattern), "\"%s\"", key);
    p = strstr(json, keyPattern);
    if (!p) return 0;

    p += strlen(keyPattern);
    while (*p && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')) p++;
    if (*p != ':') return 0;
    p++;
    while (*p && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')) p++;

    if (*p == '"') {
        p++;
        while (*p && *p != '"' && i < outSize - 1) {
            if (*p == '\\' && *(p + 1)) {
                p++;
            }
            out[i++] = *p++;
        }
        out[i] = '\0';
        return 1;
    } else {
        while (*p && *p != ',' && *p != '}' && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n' && i < outSize - 1) {
            out[i++] = *p++;
        }
        out[i] = '\0';
        return (i > 0);
    }
}

static int getBodyParam(const char *body, const char *key, char *out, int outSize) {
    if (body == NULL || key == NULL || out == NULL || outSize <= 0) return 0;
    out[0] = '\0';
    if (getJsonParam(body, key, out, outSize)) return 1;
    return getQueryParam(body, key, out, outSize);
}

static void appendStr(char *buf, int bufSize, int *pos, const char *str) {
    int len = (int)strlen(str);
    if (*pos + len >= bufSize - 1) len = bufSize - 1 - *pos;
    if (len <= 0) return;
    memcpy(buf + *pos, str, len);
    *pos += len;
    buf[*pos] = '\0';
}

static void appendJStr(char *buf, int bufSize, int *pos, const char *raw) {
    char escaped[2048];
    jsonEscape(raw, escaped, sizeof(escaped));
    appendStr(buf, bufSize, pos, "\"");
    appendStr(buf, bufSize, pos, escaped);
    appendStr(buf, bufSize, pos, "\"");
}

static int countAvailableSeats(struct Event *ptr) {
    int i, j, count = 0;
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 12; j++) {
            if (ptr->seats[i][j] != 'X') count++;
        }
    }
    return count;
}

static void buildEventJson(struct Event *ptr, char *buf, int bufSize, int *pos) {
    char tmp[64];
    int available = countAvailableSeats(ptr);

    appendStr(buf, bufSize, pos, "{");
    snprintf(tmp, sizeof(tmp), "\"id\":%d,", ptr->id);
    appendStr(buf, bufSize, pos, tmp);
    appendStr(buf, bufSize, pos, "\"title\":");
    appendJStr(buf, bufSize, pos, ptr->title);
    appendStr(buf, bufSize, pos, ",");
    snprintf(tmp, sizeof(tmp), "\"category\":%d,", ptr->category);
    appendStr(buf, bufSize, pos, tmp);
    appendStr(buf, bufSize, pos, "\"category_name\":");
    appendJStr(buf, bufSize, pos, getCategoryName(ptr->category));
    appendStr(buf, bufSize, pos, ",");
    appendStr(buf, bufSize, pos, "\"categoryName\":");
    appendJStr(buf, bufSize, pos, getCategoryName(ptr->category));
    appendStr(buf, bufSize, pos, ",");
    appendStr(buf, bufSize, pos, "\"artist\":");
    appendJStr(buf, bufSize, pos, ptr->artist);
    appendStr(buf, bufSize, pos, ",");
    appendStr(buf, bufSize, pos, "\"venue\":");
    appendJStr(buf, bufSize, pos, ptr->venue);
    appendStr(buf, bufSize, pos, ",");
    appendStr(buf, bufSize, pos, "\"time\":");
    appendJStr(buf, bufSize, pos, ptr->time);
    appendStr(buf, bufSize, pos, ",");
    snprintf(tmp, sizeof(tmp), "\"regular_price\":%.2f,\"vip_price\":%.2f,", ptr->regularPrice, ptr->vipPrice);
    appendStr(buf, bufSize, pos, tmp);
    snprintf(tmp, sizeof(tmp), "\"regularPrice\":%.2f,\"vipPrice\":%.2f,", ptr->regularPrice, ptr->vipPrice);
    appendStr(buf, bufSize, pos, tmp);
    snprintf(tmp, sizeof(tmp), "\"isApproved\":%d,", ptr->isApproved);
    appendStr(buf, bufSize, pos, tmp);
    snprintf(tmp, sizeof(tmp), "\"available_seats\":%d,", available);
    appendStr(buf, bufSize, pos, tmp);
    snprintf(tmp, sizeof(tmp), "\"availableSeats\":%d,", available);
    appendStr(buf, bufSize, pos, tmp);
    appendStr(buf, bufSize, pos, "\"isFullyBooked\":");
    appendStr(buf, bufSize, pos, (available == 0) ? "true" : "false");
    appendStr(buf, bufSize, pos, "}");
}

static void apiGetEvents(int filterMode, char *out, int outSize) {
    struct Event *ptr = head;
    int pos = 0;
    int first = 1;

    appendStr(out, outSize, &pos, "[");
    while (ptr != NULL) {
        int show = 0;
        if (filterMode == 0) show = 1;
        else if (filterMode == 1 && ptr->category == 1) show = 1;
        else if (filterMode == 2 && ptr->category >= 2) show = 1;

        if (show && ptr->isApproved == 1) {
            if (!first) appendStr(out, outSize, &pos, ",");
            buildEventJson(ptr, out, outSize, &pos);
            first = 0;
        }
        ptr = ptr->next;
    }
    appendStr(out, outSize, &pos, "]");
}

static void apiGetSeats(int eventId, char *out, int outSize) {
    struct Event *ptr = head;
    int pos = 0;
    int i, j;
    char tmp[128];
    int first = 1;

    while (ptr != NULL && ptr->id != eventId) ptr = ptr->next;

    if (ptr == NULL) {
        snprintf(out, outSize, "{\"success\":false,\"message\":\"Event not found.\"}");
        return;
    }

    appendStr(out, outSize, &pos, "{\"success\":true,");
    snprintf(tmp, sizeof(tmp), "\"id\":%d,\"eventId\":%d,", ptr->id, ptr->id);
    appendStr(out, outSize, &pos, tmp);
    appendStr(out, outSize, &pos, "\"title\":");
    appendJStr(out, outSize, &pos, ptr->title);
    appendStr(out, outSize, &pos, ",\"venue\":");
    appendJStr(out, outSize, &pos, ptr->venue);
    appendStr(out, outSize, &pos, ",\"time\":");
    appendJStr(out, outSize, &pos, ptr->time);
    snprintf(tmp, sizeof(tmp), ",\"regular_price\":%.2f,\"vip_price\":%.2f,\"regularPrice\":%.2f,\"vipPrice\":%.2f,",
             ptr->regularPrice, ptr->vipPrice, ptr->regularPrice, ptr->vipPrice);
    appendStr(out, outSize, &pos, tmp);
    appendStr(out, outSize, &pos, "\"seats\":[");

    for (i = 0; i < 10; i++) {
        for (j = 0; j < 12; j++) {
            if (!first) appendStr(out, outSize, &pos, ",");
            snprintf(tmp, sizeof(tmp), "{\"row\":%d,\"col\":%d,\"rowChar\":\"%c\",\"type\":\"%s\",\"booked\":%s}",
                     i + 1,
                     j + 1,
                     'A' + i,
                     (i < 2) ? "VIP" : "Regular",
                     (ptr->seats[i][j] == 'X') ? "true" : "false");
            appendStr(out, outSize, &pos, tmp);
            first = 0;
        }
    }
    appendStr(out, outSize, &pos, "]}");
}

struct SeatReq {
    char rowChar;
    int col;
    int rowIndex;
    int colIndex;
};

static int parseRequestedSeats(const char *body, const char *rowInput, int colInput, struct SeatReq *reqs, int maxReqs) {
    int count = 0;
    const char *p;

    if (body != NULL) {
        p = strstr(body, "\"seats\"");
        if (p != NULL) {
            p += 7;
            while (*p && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n' || *p == ':')) p++;
            if (*p == '[') {
                p++;
                while (*p && *p != ']' && count < maxReqs) {
                    while (*p && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n' || *p == ',')) p++;
                    if (*p == ']') break;
                    if (*p == '{') {
                        char rChar = '\0';
                        int cNum = 0;
                        const char *objEnd = strchr(p, '}');
                        if (!objEnd) break;
                        const char *rKey = strstr(p, "\"row\"");
                        if (rKey && rKey < objEnd) {
                            rKey += 5;
                            while (*rKey && (*rKey == ' ' || *rKey == ':' || *rKey == '"')) rKey++;
                            if (*rKey >= 'a' && *rKey <= 'z') rChar = (char)(*rKey - 'a' + 'A');
                            else if (*rKey >= 'A' && *rKey <= 'Z') rChar = *rKey;
                            else if (*rKey >= '1' && *rKey <= '9') {
                                int rn = atoi(rKey);
                                if (rn >= 1 && rn <= 10) rChar = (char)('A' + (rn - 1));
                            }
                        }
                        const char *cKey = strstr(p, "\"col\"");
                        if (!cKey || cKey > objEnd) cKey = strstr(p, "\"seat\"");
                        if (cKey && cKey < objEnd) {
                            cKey += 5;
                            while (*cKey && (*cKey == ' ' || *cKey == ':' || *cKey == '"')) cKey++;
                            cNum = atoi(cKey);
                        }
                        if (rChar >= 'A' && rChar <= 'J' && cNum >= 1 && cNum <= 12) {
                            reqs[count].rowChar = rChar;
                            reqs[count].col = cNum;
                            reqs[count].rowIndex = rChar - 'A';
                            reqs[count].colIndex = cNum - 1;
                            count++;
                        }
                        p = objEnd + 1;
                    } else if (*p == '"') {
                        p++;
                        char rChar = '\0';
                        int cNum = 0;
                        if (*p >= 'a' && *p <= 'z') rChar = (char)(*p - 'a' + 'A');
                        else if (*p >= 'A' && *p <= 'Z') rChar = *p;
                        p++;
                        if (*p == '-' || *p == '_') p++;
                        cNum = atoi(p);
                        while (*p && *p != '"') p++;
                        if (*p == '"') p++;
                        if (rChar >= 'A' && rChar <= 'J' && cNum >= 1 && cNum <= 12) {
                            reqs[count].rowChar = rChar;
                            reqs[count].col = cNum;
                            reqs[count].rowIndex = rChar - 'A';
                            reqs[count].colIndex = cNum - 1;
                            count++;
                        }
                    } else {
                        p++;
                    }
                }
            } else if (*p == '"') {
                p++;
                while (*p && *p != '"' && count < maxReqs) {
                    while (*p && (*p == ' ' || *p == ',')) p++;
                    if (*p == '"' || *p == '\0') break;
                    char rChar = '\0';
                    int cNum = 0;
                    if (*p >= 'a' && *p <= 'z') rChar = (char)(*p - 'a' + 'A');
                    else if (*p >= 'A' && *p <= 'Z') rChar = *p;
                    p++;
                    if (*p == '-' || *p == '_') p++;
                    cNum = atoi(p);
                    while (*p && isdigit((unsigned char)*p)) p++;
                    if (rChar >= 'A' && rChar <= 'J' && cNum >= 1 && cNum <= 12) {
                        reqs[count].rowChar = rChar;
                        reqs[count].col = cNum;
                        reqs[count].rowIndex = rChar - 'A';
                        reqs[count].colIndex = cNum - 1;
                        count++;
                    }
                }
            }
        }
    }

    if (count == 0 && rowInput != NULL && rowInput[0] != '\0' && colInput >= 1 && colInput <= 12) {
        char rChar = '\0';
        if (rowInput[0] >= '1' && rowInput[0] <= '9') {
            int rn = atoi(rowInput);
            if (rn >= 1 && rn <= 10) rChar = (char)('A' + (rn - 1));
        } else if (rowInput[0] >= 'a' && rowInput[0] <= 'j') {
            rChar = (char)(rowInput[0] - 'a' + 'A');
        } else if (rowInput[0] >= 'A' && rowInput[0] <= 'J') {
            rChar = rowInput[0];
        }
        if (rChar >= 'A' && rChar <= 'J') {
            reqs[0].rowChar = rChar;
            reqs[0].col = colInput;
            reqs[0].rowIndex = rChar - 'A';
            reqs[0].colIndex = colInput - 1;
            count = 1;
        }
    }

    return count;
}

static void apiBook(int eventId, const char *rowInput, int colInput, const char *buyerName, const char *body, char *out, int outSize) {
    struct Event *ptr = head;
    struct SeatReq requested[120];
    int reqCount = 0;
    int i, j;
    int createdIds[120];
    float totalPrice = 0.0f;
    struct Booking *firstNew = NULL;
    struct Booking *lastNew = NULL;
    char tmp[256];
    int pos = 0;

    while (ptr != NULL && ptr->id != eventId) ptr = ptr->next;

    if (ptr == NULL || ptr->isApproved == 0) {
        snprintf(out, outSize, "{\"success\":false,\"message\":\"Invalid event ID or event not approved.\"}");
        return;
    }

    if (!hasAvailableSeats(ptr)) {
        snprintf(out, outSize, "{\"success\":false,\"message\":\"Event is fully booked. Please join the waiting list.\",\"fullyBooked\":true}");
        return;
    }

    if (buyerName == NULL || buyerName[0] == '\0') {
        snprintf(out, outSize, "{\"success\":false,\"message\":\"Buyer name is required.\"}");
        return;
    }

    reqCount = parseRequestedSeats(body, rowInput, colInput, requested, 120);
    if (reqCount <= 0) {
        snprintf(out, outSize, "{\"success\":false,\"message\":\"No valid seats specified for booking.\"}");
        return;
    }

    if (reqCount > countAvailableSeats(ptr)) {
        snprintf(out, outSize, "{\"success\":false,\"message\":\"Requested %d seats, but only %d seats are available.\"}",
                 reqCount, countAvailableSeats(ptr));
        return;
    }

    for (i = 0; i < reqCount; i++) {
        for (j = 0; j < i; j++) {
            if (requested[i].rowChar == requested[j].rowChar && requested[i].col == requested[j].col) {
                snprintf(out, outSize, "{\"success\":false,\"message\":\"Duplicate seat %c%d specified in selection.\"}",
                         requested[i].rowChar, requested[i].col);
                return;
            }
        }
    }

    for (i = 0; i < reqCount; i++) {
        int r = requested[i].rowIndex;
        int c = requested[i].colIndex;
        if (ptr->seats[r][c] == 'X') {
            snprintf(out, outSize, "{\"success\":false,\"message\":\"Seat %c%d is already booked.\"}",
                     requested[i].rowChar, requested[i].col);
            return;
        }
    }

    for (i = 0; i < reqCount; i++) {
        struct Booking *newnode = (struct Booking *)malloc(sizeof(struct Booking));
        if (newnode == NULL) {
            for (j = 0; j < i; j++) {
                int r = requested[j].rowIndex;
                int c = requested[j].colIndex;
                ptr->seats[r][c] = (r < 2) ? 'V' : 'O';
            }
            while (firstNew != NULL) {
                struct Booking *nxt = firstNew->next;
                free(firstNew);
                firstNew = nxt;
            }
            snprintf(out, outSize, "{\"success\":false,\"message\":\"Memory allocation failed.\"}");
            return;
        }

        int r = requested[i].rowIndex;
        int c = requested[i].colIndex;
        float price = (r < 2) ? ptr->vipPrice : ptr->regularPrice;

        ptr->seats[r][c] = 'X';
        totalPrice += price;

        newnode->bookingId = bookingCounter++;
        createdIds[i] = newnode->bookingId;
        strncpy(newnode->buyerName, buyerName, 49);
        newnode->buyerName[49] = '\0';
        newnode->eventId = ptr->id;
        strncpy(newnode->eventTitle, ptr->title, 59);
        newnode->eventTitle[59] = '\0';
        newnode->category = ptr->category;
        newnode->row = requested[i].rowChar;
        newnode->col = requested[i].col;
        newnode->price = price;
        newnode->next = NULL;

        if (firstNew == NULL) {
            firstNew = newnode;
            lastNew = newnode;
        } else {
            lastNew->next = newnode;
            lastNew = newnode;
        }
    }

    if (bookingHead == NULL) {
        bookingHead = firstNew;
    } else {
        struct Booking *temp = bookingHead;
        while (temp->next != NULL) temp = temp->next;
        temp->next = firstNew;
    }

    saveAllData();

    appendStr(out, outSize, &pos, "{\"success\":true,\"message\":\"");
    snprintf(tmp, sizeof(tmp), "%d ticket(s) booked successfully!", reqCount);
    appendStr(out, outSize, &pos, tmp);
    snprintf(tmp, sizeof(tmp), "\",\"count\":%d,\"totalPrice\":%.2f,\"bookingId\":%d,\"id\":%d,\"booking_id\":%d,",
             reqCount, totalPrice, createdIds[0], createdIds[0], createdIds[0]);
    appendStr(out, outSize, &pos, tmp);

    appendStr(out, outSize, &pos, "\"bookingIds\":[");
    for (i = 0; i < reqCount; i++) {
        if (i > 0) appendStr(out, outSize, &pos, ",");
        snprintf(tmp, sizeof(tmp), "%d", createdIds[i]);
        appendStr(out, outSize, &pos, tmp);
    }
    appendStr(out, outSize, &pos, "],\"bookings\":[");

    for (i = 0; i < reqCount; i++) {
        if (i > 0) appendStr(out, outSize, &pos, ",");
        int r = requested[i].rowIndex;
        float price = (r < 2) ? ptr->vipPrice : ptr->regularPrice;
        snprintf(tmp, sizeof(tmp),
                 "{\"bookingId\":%d,\"id\":%d,\"row\":\"%c\",\"col\":%d,\"seat\":\"%c%d\",\"seatType\":\"%s\",\"price\":%.2f}",
                 createdIds[i], createdIds[i], requested[i].rowChar, requested[i].col,
                 requested[i].rowChar, requested[i].col, (r < 2) ? "VIP" : "Regular", price);
        appendStr(out, outSize, &pos, tmp);
    }
    appendStr(out, outSize, &pos, "]}");
}

static void apiCancel(int bookingId, char *out, int outSize) {
    struct Booking *ptr = bookingHead;
    struct Booking *prev = NULL;
    struct Event *eventPtr;
    int rowIndex, colIndex;

    while (ptr != NULL && ptr->bookingId != bookingId) {
        prev = ptr;
        ptr = ptr->next;
    }

    if (ptr == NULL) {
        snprintf(out, outSize, "{\"success\":false,\"message\":\"Booking ID %d not found.\"}", bookingId);
        return;
    }

    eventPtr = head;
    while (eventPtr != NULL && eventPtr->id != ptr->eventId) eventPtr = eventPtr->next;

    if (eventPtr == NULL) {
        snprintf(out, outSize, "{\"success\":false,\"message\":\"Associated event not found. Cancellation aborted.\"}");
        return;
    }

    rowIndex = ptr->row - 'A';
    colIndex = ptr->col - 1;

    if (rowIndex < 0 || rowIndex >= 10 || colIndex < 0 || colIndex >= 12) {
        snprintf(out, outSize, "{\"success\":false,\"message\":\"Invalid seat data in booking record.\"}");
        return;
    }

    if (eventPtr->waitlistFront != NULL) {
        struct Booking *autoBooking = (struct Booking *)malloc(sizeof(struct Booking));
        struct WaitlistNode *qTemp;
        float autoPrice;

        if (autoBooking == NULL) {
            snprintf(out, outSize, "{\"success\":false,\"message\":\"Memory allocation failed for auto-booking.\"}");
            return;
        }

        autoPrice = (rowIndex < 2) ? eventPtr->vipPrice : eventPtr->regularPrice;
        autoBooking->bookingId = bookingCounter++;
        strncpy(autoBooking->buyerName, eventPtr->waitlistFront->buyerName, 49);
        autoBooking->buyerName[49] = '\0';
        autoBooking->eventId = eventPtr->id;
        strncpy(autoBooking->eventTitle, eventPtr->title, 59);
        autoBooking->eventTitle[59] = '\0';
        autoBooking->category = eventPtr->category;
        autoBooking->row = ptr->row;
        autoBooking->col = ptr->col;
        autoBooking->price = autoPrice;
        autoBooking->next = NULL;

        if (!pushCancellation(ptr, 1, autoBooking->bookingId, eventPtr->waitlistFront->buyerName, eventPtr->waitlistFront->token)) {
            free(autoBooking);
            snprintf(out, outSize, "{\"success\":false,\"message\":\"Could not push to undo stack. Cancellation aborted.\"}");
            return;
        }

        if (bookingHead == NULL) {
            bookingHead = autoBooking;
        } else {
            struct Booking *bTemp = bookingHead;
            while (bTemp->next != NULL) bTemp = bTemp->next;
            bTemp->next = autoBooking;
        }

        qTemp = eventPtr->waitlistFront;
        eventPtr->waitlistFront = eventPtr->waitlistFront->next;
        if (eventPtr->waitlistFront == NULL) eventPtr->waitlistRear = NULL;

        if (prev == NULL) bookingHead = ptr->next;
        else prev->next = ptr->next;

        free(qTemp);
        free(ptr);

        snprintf(out, outSize,
                 "{\"success\":true,\"message\":\"Ticket cancelled and seat auto-allocated to waiting list.\",\"waitlistAllocated\":true,\"waitlist_allocated\":true,\"autoAllocated\":true}");
    } else {
        if (!pushCancellation(ptr, 0, 0, "", 0)) {
            snprintf(out, outSize, "{\"success\":false,\"message\":\"Could not push to undo stack. Cancellation aborted.\"}");
            return;
        }

        eventPtr->seats[rowIndex][colIndex] = (rowIndex < 2) ? 'V' : 'O';

        if (prev == NULL) bookingHead = ptr->next;
        else prev->next = ptr->next;

        free(ptr);

        snprintf(out, outSize,
                 "{\"success\":true,\"message\":\"Booking cancelled successfully.\",\"waitlistAllocated\":false,\"waitlist_allocated\":false,\"autoAllocated\":false}");
    }

    saveAllData();
}

static void apiUndo(char *out, int outSize) {
    if (top == NULL) {
        snprintf(out, outSize, "{\"success\":false,\"message\":\"No cancellations available to undo.\"}");
        return;
    }

    undoCancellation();
    snprintf(out, outSize, "{\"success\":true,\"message\":\"Last cancellation undone successfully.\"}");
}

static void apiGetBookings(char *out, int outSize) {
    struct Booking *ptr = bookingHead;
    int pos = 0;
    int first = 1;
    char tmp[256];

    appendStr(out, outSize, &pos, "[");
    while (ptr != NULL) {
        if (!first) appendStr(out, outSize, &pos, ",");
        appendStr(out, outSize, &pos, "{");
        snprintf(tmp, sizeof(tmp), "\"id\":%d,\"booking_id\":%d,\"bookingId\":%d,",
                 ptr->bookingId, ptr->bookingId, ptr->bookingId);
        appendStr(out, outSize, &pos, tmp);
        appendStr(out, outSize, &pos, "\"buyer_name\":");
        appendJStr(out, outSize, &pos, ptr->buyerName);
        appendStr(out, outSize, &pos, ",\"buyerName\":");
        appendJStr(out, outSize, &pos, ptr->buyerName);
        appendStr(out, outSize, &pos, ",\"event_id\":");
        snprintf(tmp, sizeof(tmp), "%d,", ptr->eventId);
        appendStr(out, outSize, &pos, tmp);
        appendStr(out, outSize, &pos, "\"eventId\":");
        snprintf(tmp, sizeof(tmp), "%d,", ptr->eventId);
        appendStr(out, outSize, &pos, tmp);
        appendStr(out, outSize, &pos, "\"event_title\":");
        appendJStr(out, outSize, &pos, ptr->eventTitle);
        appendStr(out, outSize, &pos, ",\"eventTitle\":");
        appendJStr(out, outSize, &pos, ptr->eventTitle);
        appendStr(out, outSize, &pos, ",\"categoryName\":");
        appendJStr(out, outSize, &pos, getCategoryName(ptr->category));
        snprintf(tmp, sizeof(tmp), ",\"row\":\"%c\",\"col\":%d,", ptr->row, ptr->col);
        appendStr(out, outSize, &pos, tmp);
        appendStr(out, outSize, &pos, "\"seatType\":");
        appendJStr(out, outSize, &pos, (ptr->row == 'A' || ptr->row == 'B') ? "VIP" : "Regular");
        snprintf(tmp, sizeof(tmp), ",\"price\":%.2f}", ptr->price);
        appendStr(out, outSize, &pos, tmp);
        first = 0;
        ptr = ptr->next;
    }
    appendStr(out, outSize, &pos, "]");
}

static void apiGetWaitlist(int eventId, char *out, int outSize) {
    struct Event *ptr = head;
    struct WaitlistNode *wptr;
    int pos = 0;
    int first = 1;
    char tmp[128];

    while (ptr != NULL && ptr->id != eventId) ptr = ptr->next;

    if (ptr == NULL) {
        snprintf(out, outSize, "{\"success\":false,\"message\":\"Event not found.\"}");
        return;
    }

    appendStr(out, outSize, &pos, "{\"success\":true,");
    snprintf(tmp, sizeof(tmp), "\"id\":%d,\"eventId\":%d,", ptr->id, ptr->id);
    appendStr(out, outSize, &pos, tmp);
    appendStr(out, outSize, &pos, "\"title\":");
    appendJStr(out, outSize, &pos, ptr->title);
    appendStr(out, outSize, &pos, ",\"queue\":[");

    wptr = ptr->waitlistFront;
    while (wptr != NULL) {
        if (!first) appendStr(out, outSize, &pos, ",");
        appendStr(out, outSize, &pos, "{");
        snprintf(tmp, sizeof(tmp), "\"id\":%d,\"token\":%d,", wptr->token, wptr->token);
        appendStr(out, outSize, &pos, tmp);
        appendStr(out, outSize, &pos, "\"buyer_name\":");
        appendJStr(out, outSize, &pos, wptr->buyerName);
        appendStr(out, outSize, &pos, ",\"buyerName\":");
        appendJStr(out, outSize, &pos, wptr->buyerName);
        appendStr(out, outSize, &pos, "}");
        first = 0;
        wptr = wptr->next;
    }
    appendStr(out, outSize, &pos, "]}");
}

static void apiJoinWaitlist(int eventId, const char *buyerName, char *out, int outSize) {
    struct Event *ptr = head;
    struct WaitlistNode *newnode;
    char tmp[128];
    int pos = 0;

    while (ptr != NULL && ptr->id != eventId) ptr = ptr->next;

    if (ptr == NULL || ptr->isApproved == 0) {
        snprintf(out, outSize, "{\"success\":false,\"message\":\"Invalid event ID or event not approved.\"}");
        return;
    }

    if (hasAvailableSeats(ptr)) {
        snprintf(out, outSize, "{\"success\":false,\"message\":\"This event still has available seats. Please book directly.\"}");
        return;
    }

    if (buyerName == NULL || buyerName[0] == '\0') {
        snprintf(out, outSize, "{\"success\":false,\"message\":\"Buyer name is required.\"}");
        return;
    }

    newnode = (struct WaitlistNode *)malloc(sizeof(struct WaitlistNode));
    if (newnode == NULL) {
        snprintf(out, outSize, "{\"success\":false,\"message\":\"Memory allocation failed.\"}");
        return;
    }

    newnode->token = waitTokenCounter++;
    newnode->eventId = ptr->id;
    strncpy(newnode->buyerName, buyerName, 49);
    newnode->buyerName[49] = '\0';
    newnode->next = NULL;

    if (ptr->waitlistRear == NULL) {
        ptr->waitlistFront = ptr->waitlistRear = newnode;
    } else {
        ptr->waitlistRear->next = newnode;
        ptr->waitlistRear = newnode;
    }

    saveAllData();

    appendStr(out, outSize, &pos, "{\"success\":true,\"message\":\"Added to waiting list successfully!\",");
    snprintf(tmp, sizeof(tmp), "\"token\":%d,", newnode->token);
    appendStr(out, outSize, &pos, tmp);
    appendStr(out, outSize, &pos, "\"buyerName\":");
    appendJStr(out, outSize, &pos, newnode->buyerName);
    appendStr(out, outSize, &pos, ",\"eventTitle\":");
    appendJStr(out, outSize, &pos, ptr->title);
    appendStr(out, outSize, &pos, "}");
}

static void apiSearch(const char *type, const char *value, char *out, int outSize) {
    int pos = 0;
    int first = 1;
    char tmp[256];

    appendStr(out, outSize, &pos, "[");

    if (strcmp(type, "category") == 0) {
        int category = atoi(value);
        struct Event *ptr = head;
        while (ptr != NULL) {
            if (ptr->category == category && ptr->isApproved == 1) {
                if (!first) appendStr(out, outSize, &pos, ",");
                buildEventJson(ptr, out, outSize, &pos);
                first = 0;
            }
            ptr = ptr->next;
        }
    } else if (strcmp(type, "artist") == 0) {
        struct Event *ptr = head;
        char vLower[100], aLower[100];
        int i;
        for (i = 0; value[i] && i < 99; i++) vLower[i] = (value[i] >= 'A' && value[i] <= 'Z') ? value[i]-'A'+'a' : value[i];
        vLower[i] = '\0';
        while (ptr != NULL) {
            for (i = 0; ptr->artist[i] && i < 99; i++) aLower[i] = (ptr->artist[i] >= 'A' && ptr->artist[i] <= 'Z') ? ptr->artist[i]-'A'+'a' : ptr->artist[i];
            aLower[i] = '\0';
            if (ptr->isApproved == 1 && strstr(aLower, vLower) != NULL) {
                if (!first) appendStr(out, outSize, &pos, ",");
                buildEventJson(ptr, out, outSize, &pos);
                first = 0;
            }
            ptr = ptr->next;
        }
    } else if (strcmp(type, "title") == 0 || strcmp(type, "event") == 0) {
        struct Event *found = binarySearchEventByTitle(value);
        if (found != NULL) {
            buildEventJson(found, out, outSize, &pos);
        } else {
            struct Event *ptr = head;
            char vLower[100], tLower[100];
            int i;
            for (i = 0; value[i] && i < 99; i++) vLower[i] = (value[i] >= 'A' && value[i] <= 'Z') ? value[i]-'A'+'a' : value[i];
            vLower[i] = '\0';
            while (ptr != NULL) {
                for (i = 0; ptr->title[i] && i < 99; i++) tLower[i] = (ptr->title[i] >= 'A' && ptr->title[i] <= 'Z') ? ptr->title[i]-'A'+'a' : ptr->title[i];
                tLower[i] = '\0';
                if (ptr->isApproved == 1 && strstr(tLower, vLower) != NULL) {
                    if (!first) appendStr(out, outSize, &pos, ",");
                    buildEventJson(ptr, out, outSize, &pos);
                    first = 0;
                }
                ptr = ptr->next;
            }
        }
    } else if (strcmp(type, "bookingId") == 0 || strcmp(type, "booking") == 0) {
        int bookingId = atoi(value);
        struct Booking *ptr = bookingHead;
        while (ptr != NULL) {
            if (ptr->bookingId == bookingId) {
                if (!first) appendStr(out, outSize, &pos, ",");
                appendStr(out, outSize, &pos, "{");
                snprintf(tmp, sizeof(tmp), "\"id\":%d,\"booking_id\":%d,\"bookingId\":%d,",
                         ptr->bookingId, ptr->bookingId, ptr->bookingId);
                appendStr(out, outSize, &pos, tmp);
                appendStr(out, outSize, &pos, "\"buyer_name\":");
                appendJStr(out, outSize, &pos, ptr->buyerName);
                appendStr(out, outSize, &pos, ",\"buyerName\":");
                appendJStr(out, outSize, &pos, ptr->buyerName);
                appendStr(out, outSize, &pos, ",\"event_title\":");
                appendJStr(out, outSize, &pos, ptr->eventTitle);
                appendStr(out, outSize, &pos, ",\"eventTitle\":");
                appendJStr(out, outSize, &pos, ptr->eventTitle);
                appendStr(out, outSize, &pos, ",\"categoryName\":");
                appendJStr(out, outSize, &pos, getCategoryName(ptr->category));
                snprintf(tmp, sizeof(tmp), ",\"row\":\"%c\",\"col\":%d,", ptr->row, ptr->col);
                appendStr(out, outSize, &pos, tmp);
                appendStr(out, outSize, &pos, "\"seatType\":");
                appendJStr(out, outSize, &pos, (ptr->row == 'A' || ptr->row == 'B') ? "VIP" : "Regular");
                snprintf(tmp, sizeof(tmp), ",\"price\":%.2f}", ptr->price);
                appendStr(out, outSize, &pos, tmp);
                first = 0;
                break;
            }
            ptr = ptr->next;
        }
    } else if (strcmp(type, "buyer") == 0) {
        struct Booking *ptr = bookingHead;
        char vLower[100], bLower[100];
        int i;
        for (i = 0; value[i] && i < 99; i++) vLower[i] = (value[i] >= 'A' && value[i] <= 'Z') ? value[i]-'A'+'a' : value[i];
        vLower[i] = '\0';
        while (ptr != NULL) {
            for (i = 0; ptr->buyerName[i] && i < 99; i++) bLower[i] = (ptr->buyerName[i] >= 'A' && ptr->buyerName[i] <= 'Z') ? ptr->buyerName[i]-'A'+'a' : ptr->buyerName[i];
            bLower[i] = '\0';
            if (strstr(bLower, vLower) != NULL) {
                if (!first) appendStr(out, outSize, &pos, ",");
                appendStr(out, outSize, &pos, "{");
                snprintf(tmp, sizeof(tmp), "\"id\":%d,\"booking_id\":%d,\"bookingId\":%d,",
                         ptr->bookingId, ptr->bookingId, ptr->bookingId);
                appendStr(out, outSize, &pos, tmp);
                appendStr(out, outSize, &pos, "\"buyer_name\":");
                appendJStr(out, outSize, &pos, ptr->buyerName);
                appendStr(out, outSize, &pos, ",\"buyerName\":");
                appendJStr(out, outSize, &pos, ptr->buyerName);
                appendStr(out, outSize, &pos, ",\"event_title\":");
                appendJStr(out, outSize, &pos, ptr->eventTitle);
                appendStr(out, outSize, &pos, ",\"eventTitle\":");
                appendJStr(out, outSize, &pos, ptr->eventTitle);
                appendStr(out, outSize, &pos, ",\"categoryName\":");
                appendJStr(out, outSize, &pos, getCategoryName(ptr->category));
                snprintf(tmp, sizeof(tmp), ",\"row\":\"%c\",\"col\":%d,", ptr->row, ptr->col);
                appendStr(out, outSize, &pos, tmp);
                appendStr(out, outSize, &pos, "\"seatType\":");
                appendJStr(out, outSize, &pos, (ptr->row == 'A' || ptr->row == 'B') ? "VIP" : "Regular");
                snprintf(tmp, sizeof(tmp), ",\"price\":%.2f}", ptr->price);
                appendStr(out, outSize, &pos, tmp);
                first = 0;
            }
            ptr = ptr->next;
        }
    }

    appendStr(out, outSize, &pos, "]");
}

static void apiGetOverview(char *out, int outSize) {
    struct Event *eptr = head;
    struct Booking *bptr = bookingHead;
    int totalEvents = 0, commercialEvents = 0, regionalEvents = 0;
    int totalBookings = 0, commercialBookings = 0, regionalBookings = 0;
    float totalRevenue = 0.0f, commercialRevenue = 0.0f, regionalRevenue = 0.0f;
    int stackDepth = 0;
    int seatsOccupied = 0;
    int waitlistEntries = 0;
    struct StackNode *sptr = top;
    char tmp[256];
    int pos = 0;
    int i, j;

    while (eptr != NULL) {
        totalEvents++;
        if (eptr->category == 1) commercialEvents++;
        else regionalEvents++;

        for (i = 0; i < 10; i++) {
            for (j = 0; j < 12; j++) {
                if (eptr->seats[i][j] == 'X') seatsOccupied++;
            }
        }

        struct WaitlistNode *w = eptr->waitlistFront;
        while (w != NULL) {
            waitlistEntries++;
            w = w->next;
        }

        eptr = eptr->next;
    }

    while (bptr != NULL) {
        totalBookings++;
        totalRevenue += bptr->price;
        if (bptr->category == 1) { commercialBookings++; commercialRevenue += bptr->price; }
        else { regionalBookings++; regionalRevenue += bptr->price; }
        bptr = bptr->next;
    }

    while (sptr != NULL) { stackDepth++; sptr = sptr->next; }

    appendStr(out, outSize, &pos, "{\"success\":true,");
    snprintf(tmp, sizeof(tmp), "\"total_events\":%d,\"totalEvents\":%d,\"commercialEvents\":%d,\"regionalEvents\":%d,",
             totalEvents, totalEvents, commercialEvents, regionalEvents);
    appendStr(out, outSize, &pos, tmp);
    snprintf(tmp, sizeof(tmp), "\"total_bookings\":%d,\"totalBookings\":%d,\"commercialBookings\":%d,\"regionalBookings\":%d,",
             totalBookings, totalBookings, commercialBookings, regionalBookings);
    appendStr(out, outSize, &pos, tmp);
    snprintf(tmp, sizeof(tmp), "\"seats_occupied\":%d,\"seatsOccupied\":%d,\"waitlist_entries\":%d,\"waitlistEntries\":%d,",
             seatsOccupied, seatsOccupied, waitlistEntries, waitlistEntries);
    appendStr(out, outSize, &pos, tmp);
    snprintf(tmp, sizeof(tmp), "\"total_revenue\":%.2f,\"totalRevenue\":%.2f,\"revenue\":%.2f,\"commercialRevenue\":%.2f,\"regionalRevenue\":%.2f,",
             totalRevenue, totalRevenue, totalRevenue, commercialRevenue, regionalRevenue);
    appendStr(out, outSize, &pos, tmp);
    snprintf(tmp, sizeof(tmp), "\"undoStackDepth\":%d}", stackDepth);
    appendStr(out, outSize, &pos, tmp);
}

static void apiAddEvent(int category, const char *title, const char *artist,
                        const char *venue, const char *time,
                        float regularPrice, float vipPrice,
                        char *out, int outSize) {
    int pos = 0;
    char tmp[256];

    if (category < 1 || category > 5) {
        snprintf(out, outSize, "{\"success\":false,\"message\":\"Category must be between 1 and 5.\"}");
        return;
    }
    if (title == NULL || title[0] == '\0') {
        snprintf(out, outSize, "{\"success\":false,\"message\":\"Title is required.\"}");
        return;
    }
    if (artist == NULL || artist[0] == '\0') {
        snprintf(out, outSize, "{\"success\":false,\"message\":\"Artist/Studio name is required.\"}");
        return;
    }
    if (regularPrice <= 0 || vipPrice <= 0) {
        snprintf(out, outSize, "{\"success\":false,\"message\":\"Prices must be greater than zero.\"}");
        return;
    }

    int newId = eventIdCounter++;
    char titleBuf[60], artistBuf[60], venueBuf[60], timeBuf[20];
    strncpy(titleBuf, title, 59);  titleBuf[59] = '\0';
    strncpy(artistBuf, artist, 59); artistBuf[59] = '\0';
    strncpy(venueBuf, venue ? venue : "Main Auditorium", 59); venueBuf[59] = '\0';
    strncpy(timeBuf, time ? time : "07:00 PM", 19);  timeBuf[19] = '\0';

    addEvent(newId, titleBuf, category, artistBuf, venueBuf, timeBuf, regularPrice, vipPrice, 1);
    saveAllData();

    appendStr(out, outSize, &pos, "{\"success\":true,\"message\":\"Event registered successfully!\",");
    snprintf(tmp, sizeof(tmp), "\"id\":%d,\"eventId\":%d,", newId, newId);
    appendStr(out, outSize, &pos, tmp);
    appendStr(out, outSize, &pos, "\"title\":");
    appendJStr(out, outSize, &pos, titleBuf);
    appendStr(out, outSize, &pos, ",\"categoryName\":");
    appendJStr(out, outSize, &pos, getCategoryName(category));
    appendStr(out, outSize, &pos, "}");
}

static void sendHttpResponse(SOCKET client, int status, const char *contentType,
                              const char *body, int bodyLen) {
    char header[512];
    const char *statusText = (status == 200) ? "OK" :
                             (status == 404) ? "Not Found" :
                             (status == 400) ? "Bad Request" : "Internal Server Error";
    int headerLen;

    headerLen = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "Connection: close\r\n"
        "\r\n",
        status, statusText, contentType, bodyLen);

    send(client, header, headerLen, 0);
    if (bodyLen > 0) send(client, body, bodyLen, 0);
}

static void serveStaticFile(SOCKET client, const char *path) {
    FILE *fp;
    char filePath[512];
    char *buf;
    long fileSize;
    int bytesRead;
    const char *ct = "text/plain";

    if (strstr(path, "..") != NULL) {
        const char *msg = "{\"error\":\"Forbidden\"}";
        sendHttpResponse(client, 400, "application/json", msg, (int)strlen(msg));
        return;
    }

    if (path[0] == '\0' || strcmp(path, "/") == 0) {
        snprintf(filePath, sizeof(filePath), "frontend/index.html");
    } else {
        snprintf(filePath, sizeof(filePath), "frontend%s", path);
    }

    if (strstr(filePath, ".html")) ct = "text/html; charset=utf-8";
    else if (strstr(filePath, ".css")) ct = "text/css; charset=utf-8";
    else if (strstr(filePath, ".js")) ct = "application/javascript; charset=utf-8";
    else if (strstr(filePath, ".json")) ct = "application/json; charset=utf-8";

    fp = fopen(filePath, "rb");
    if (fp == NULL) {
        const char *msg = "404 Not Found";
        sendHttpResponse(client, 404, "text/plain", msg, (int)strlen(msg));
        return;
    }

    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (fileSize <= 0) {
        fclose(fp);
        sendHttpResponse(client, 200, ct, "", 0);
        return;
    }

    buf = (char *)malloc((size_t)fileSize);
    if (buf == NULL) {
        fclose(fp);
        sendHttpResponse(client, 500, "text/plain", "Out of memory", 13);
        return;
    }

    bytesRead = (int)fread(buf, 1, (size_t)fileSize, fp);
    fclose(fp);
    sendHttpResponse(client, 200, ct, buf, bytesRead);
    free(buf);
}

static void handleRequest(SOCKET client, const char *req, int reqLen) {
    char method[8] = {0};
    char fullPath[1024] = {0};
    char path[512] = {0};
    char query[512] = {0};
    char body[4096] = {0};
    char jsonBuf[MAX_JSON];
    char *qmark;
    const char *bodyStart;
    int contentLength = 0;

    sscanf(req, "%7s %1023s", method, fullPath);

    qmark = strchr(fullPath, '?');
    if (qmark != NULL) {
        int pathLen = (int)(qmark - fullPath);
        if (pathLen >= (int)sizeof(path)) pathLen = (int)sizeof(path) - 1;
        strncpy(path, fullPath, pathLen);
        path[pathLen] = '\0';
        strncpy(query, qmark + 1, sizeof(query) - 1);
        query[sizeof(query) - 1] = '\0';
    } else {
        strncpy(path, fullPath, sizeof(path) - 1);
        path[sizeof(path) - 1] = '\0';
    }

    if (strcmp(method, "OPTIONS") == 0) {
        sendHttpResponse(client, 200, "text/plain", "", 0);
        return;
    }

    if (strcmp(method, "POST") == 0) {
        int bodyLen = 0;
        const char *clHeader = strstr(req, "Content-Length:");
        if (clHeader) {
            contentLength = atoi(clHeader + 15);
            if (contentLength < 0 || contentLength >= (int)sizeof(body)) contentLength = (int)sizeof(body) - 1;
        }

        if (strstr(req, "Expect: 100-continue") != NULL || strstr(req, "Expect: 100-Continue") != NULL) {
            const char *cont = "HTTP/1.1 100 Continue\r\n\r\n";
            send(client, cont, (int)strlen(cont), 0);
        }

        bodyStart = strstr(req, "\r\n\r\n");
        if (bodyStart) {
            int headerLen = (int)(bodyStart - req) + 4;
            int available = reqLen - headerLen;
            if (available > 0) {
                if (available > (int)sizeof(body) - 1) available = (int)sizeof(body) - 1;
                memcpy(body, bodyStart + 4, available);
                bodyLen = available;
            }
            while (contentLength > 0 && bodyLen < contentLength && bodyLen < (int)sizeof(body) - 1) {
                int n = recv(client, body + bodyLen, contentLength - bodyLen, 0);
                if (n <= 0) break;
                bodyLen += n;
            }
            body[bodyLen] = '\0';
        }
    }

    if (strncmp(path, "/api/", 5) != 0) {
        serveStaticFile(client, path);
        return;
    }

    const char *apiPath = path + 4;

    if (strcmp(method, "GET") == 0 && strcmp(apiPath, "/events") == 0) {
        char filterStr[16] = "0";
        getQueryParam(query, "filter", filterStr, sizeof(filterStr));
        apiGetEvents(atoi(filterStr), jsonBuf, sizeof(jsonBuf));
        sendHttpResponse(client, 200, "application/json", jsonBuf, (int)strlen(jsonBuf));

    } else if (strcmp(method, "GET") == 0 && strcmp(apiPath, "/seats") == 0) {
        char eventIdStr[16] = "0";
        getQueryParam(query, "eventId", eventIdStr, sizeof(eventIdStr));
        apiGetSeats(atoi(eventIdStr), jsonBuf, sizeof(jsonBuf));
        sendHttpResponse(client, 200, "application/json", jsonBuf, (int)strlen(jsonBuf));

    } else if (strcmp(method, "GET") == 0 && strcmp(apiPath, "/bookings") == 0) {
        apiGetBookings(jsonBuf, sizeof(jsonBuf));
        sendHttpResponse(client, 200, "application/json", jsonBuf, (int)strlen(jsonBuf));

    } else if (strcmp(method, "POST") == 0 && strcmp(apiPath, "/book") == 0) {
        char eventIdStr[16] = {0};
        char rowStr[16] = {0};
        char colStr[16] = {0};
        char buyerName[64] = {0};
        getBodyParam(body, "eventId", eventIdStr, sizeof(eventIdStr));
        getBodyParam(body, "row", rowStr, sizeof(rowStr));
        getBodyParam(body, "col", colStr, sizeof(colStr));
        if (!getBodyParam(body, "buyerName", buyerName, sizeof(buyerName))) {
            if (!getBodyParam(body, "customerName", buyerName, sizeof(buyerName))) {
                getBodyParam(body, "name", buyerName, sizeof(buyerName));
            }
        }
        apiBook(atoi(eventIdStr), rowStr, atoi(colStr), buyerName, body, jsonBuf, sizeof(jsonBuf));
        sendHttpResponse(client, 200, "application/json", jsonBuf, (int)strlen(jsonBuf));

    } else if (strcmp(method, "POST") == 0 && strcmp(apiPath, "/cancel") == 0) {
        char bookingIdStr[16] = {0};
        if (!getBodyParam(body, "bookingId", bookingIdStr, sizeof(bookingIdStr))) {
            getBodyParam(body, "id", bookingIdStr, sizeof(bookingIdStr));
        }
        apiCancel(atoi(bookingIdStr), jsonBuf, sizeof(jsonBuf));
        sendHttpResponse(client, 200, "application/json", jsonBuf, (int)strlen(jsonBuf));

    } else if (strcmp(method, "POST") == 0 && strcmp(apiPath, "/undo") == 0) {
        apiUndo(jsonBuf, sizeof(jsonBuf));
        sendHttpResponse(client, 200, "application/json", jsonBuf, (int)strlen(jsonBuf));

    } else if (strcmp(method, "GET") == 0 && strcmp(apiPath, "/waitlist") == 0) {
        char eventIdStr[16] = "0";
        getQueryParam(query, "eventId", eventIdStr, sizeof(eventIdStr));
        apiGetWaitlist(atoi(eventIdStr), jsonBuf, sizeof(jsonBuf));
        sendHttpResponse(client, 200, "application/json", jsonBuf, (int)strlen(jsonBuf));

    } else if (strcmp(method, "POST") == 0 && strcmp(apiPath, "/waitlist") == 0) {
        char eventIdStr[16] = {0};
        char buyerName[64] = {0};
        getBodyParam(body, "eventId", eventIdStr, sizeof(eventIdStr));
        if (!getBodyParam(body, "buyerName", buyerName, sizeof(buyerName))) {
            if (!getBodyParam(body, "customerName", buyerName, sizeof(buyerName))) {
                getBodyParam(body, "name", buyerName, sizeof(buyerName));
            }
        }
        apiJoinWaitlist(atoi(eventIdStr), buyerName, jsonBuf, sizeof(jsonBuf));
        sendHttpResponse(client, 200, "application/json", jsonBuf, (int)strlen(jsonBuf));

    } else if (strcmp(method, "GET") == 0 && strcmp(apiPath, "/search") == 0) {
        char type[32] = {0};
        char value[128] = {0};
        getQueryParam(query, "type", type, sizeof(type));
        getQueryParam(query, "value", value, sizeof(value));
        apiSearch(type, value, jsonBuf, sizeof(jsonBuf));
        sendHttpResponse(client, 200, "application/json", jsonBuf, (int)strlen(jsonBuf));

    } else if (strcmp(method, "GET") == 0 && strcmp(apiPath, "/overview") == 0) {
        apiGetOverview(jsonBuf, sizeof(jsonBuf));
        sendHttpResponse(client, 200, "application/json", jsonBuf, (int)strlen(jsonBuf));

    } else if (strcmp(method, "POST") == 0 && strcmp(apiPath, "/events") == 0) {
        char catStr[16] = "1";
        char title[64] = {0};
        char artist[64] = {0};
        char venue[64] = {0};
        char time[32] = {0};
        char regStr[16] = "0";
        char vipStr[16] = "0";
        getBodyParam(body, "category", catStr, sizeof(catStr));
        getBodyParam(body, "title", title, sizeof(title));
        getBodyParam(body, "artist", artist, sizeof(artist));
        getBodyParam(body, "venue", venue, sizeof(venue));
        getBodyParam(body, "time", time, sizeof(time));
        if (!getBodyParam(body, "regular_price", regStr, sizeof(regStr))) {
            getBodyParam(body, "regularPrice", regStr, sizeof(regStr));
        }
        if (!getBodyParam(body, "vip_price", vipStr, sizeof(vipStr))) {
            getBodyParam(body, "vipPrice", vipStr, sizeof(vipStr));
        }
        apiAddEvent(atoi(catStr), title, artist, venue, time,
                    (float)atof(regStr), (float)atof(vipStr),
                    jsonBuf, sizeof(jsonBuf));
        sendHttpResponse(client, 200, "application/json", jsonBuf, (int)strlen(jsonBuf));

    } else {
        const char *msg = "{\"success\":false,\"message\":\"Unknown endpoint.\"}";
        sendHttpResponse(client, 404, "application/json", msg, (int)strlen(msg));
    }
}

int main(void) {
    WSADATA wsa;
    SOCKET serverSock, clientSock;
    struct sockaddr_in addr;
    char *reqBuf;
    int recvLen;
    int opt = 1;

    if (!loadAllData()) {
        printf("Cannot start: data files corrupted or unreadable.\n");
        printf("Restore from backup and try again.\n");
        return 1;
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock == INVALID_SOCKET) {
        printf("Socket creation failed.\n");
        WSACleanup();
        return 1;
    }

    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(serverSock, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
        printf("Bind failed. Port %d may be in use.\n", PORT);
        closesocket(serverSock);
        WSACleanup();
        return 1;
    }

    if (listen(serverSock, 10) == SOCKET_ERROR) {
        printf("Listen failed.\n");
        closesocket(serverSock);
        WSACleanup();
        return 1;
    }

    printf("============================================================\n");
    printf("  Movie & Theatre Ticket Booking System - Web Server\n");
    printf("============================================================\n");
    printf("  Server running at: http://localhost:%d\n", PORT);
    printf("  Open the above URL in your browser.\n");
    printf("  Press Ctrl+C to stop the server.\n");
    printf("============================================================\n");

    reqBuf = (char *)malloc(MAX_REQ);
    if (reqBuf == NULL) {
        printf("Memory allocation failed.\n");
        closesocket(serverSock);
        WSACleanup();
        return 1;
    }

    while (1) {
        DWORD timeout = 4000;
        clientSock = accept(serverSock, NULL, NULL);
        if (clientSock == INVALID_SOCKET) continue;

        setsockopt(clientSock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
        setsockopt(clientSock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));

        memset(reqBuf, 0, MAX_REQ);
        recvLen = recv(clientSock, reqBuf, MAX_REQ - 1, 0);
        if (recvLen > 0) {
            reqBuf[recvLen] = '\0';
            handleRequest(clientSock, reqBuf, recvLen);
        }

        closesocket(clientSock);
    }

    free(reqBuf);
    closesocket(serverSock);
    WSACleanup();
    return 0;
}
