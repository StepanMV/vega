#include <stdio.h>
#include <string.h>

#include "lab3.h"

unsigned long g_Id = 1;

static void *exitMalloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        printf("Error: malloc failed\n");
        exit(1);
    }
    return ptr;
}

static void *exitRealloc(void *ptr, size_t size) {
    void *newPtr = realloc(ptr, size);
    if (newPtr == NULL) {
        printf("Error: realloc failed\n");
        exit(1);
    }
    return newPtr;
}

static Student *sortedStudents(Student *students, unsigned int count) {
    for (unsigned int i = 0; i < count; i++) {
        int min = i;
        for (unsigned int j = i + 1; j < count; j++) {
            if (strcasecmp(students[j].surname, students[min].surname) < 0) {
                min = j;
            }
        }
        Student temp = students[i];
        students[i] = students[min];
        students[min] = temp;
    }
    return students;
}

static Group *createGroup(char *name) {
    Group *group = (Group *) exitMalloc(sizeof(Group));
    strcpy(group->name, name);
    group->students = NULL;
    group->studentsCount = 0;
    return group;
}

University *initUniversity(const char *fileName) {
    University *university = (University *) exitMalloc(sizeof(University));
    university->groups = (Group *) exitMalloc(sizeof(Group));
    university->groupsCount = 0;
    FILE *file = fopen(fileName, "rb");
    if (file == NULL) {
        return university;
    }
    Student *student = (Student *) exitMalloc(sizeof(Student));
    size_t readBytes = fread(student, sizeof(Student), 1, file);
    unsigned long maxId = 0;
    while (readBytes != 0) {
        Group *group = getGroup(university, student->groupName);
        if (group == NULL) {
            Group *newGroup = createGroup(student->groupName);
            addNewGroup(university, *newGroup);
            free(newGroup);
            group = getGroup(university, student->groupName);
        }
        addNewStudent(group, *student);
        if (student->id > maxId) {
            maxId = student->id;
        }
        readBytes = fread(student, sizeof(Student), 1, file);
    }
    g_Id = maxId + 1;
    free(student);
    fclose(file);
    return university;
}

bool addNewGroup(University *university, const Group group) {
    if (university == NULL) {
        return false;
    }
    university->groupsCount++;
    university->groups = (Group *) exitRealloc(university->groups, university->groupsCount * sizeof(Group));
    university->groups[university->groupsCount - 1] = group;
    return true;
}

bool addNewStudent(Group *group, Student student) {
    if (group == NULL || strcmp(student.groupName, group->name) != 0) {
        return false;
    }
    if (student.id == 0) {
        student.id = g_Id;
        g_Id++;
    }
    group->studentsCount++;
    group->students = (Student *) exitRealloc(group->students, group->studentsCount * sizeof(Student));
    group->students[group->studentsCount - 1] = student;
    group->students = sortedStudents(group->students, group->studentsCount);
    return true;
}

bool removeGroup(University *university, const char *name) {
    if (university == NULL) {
        return false;
    }
    Group *group = getGroup(university, name);
    if (group == NULL) {
        return false;
    }
    free(group->students);
    for (unsigned int i = 0; i < university->groupsCount; i++) {
        if (strcmp(university->groups[i].name, name) == 0) {
            for (unsigned int j = i; j < university->groupsCount - 1; j++) {
                university->groups[j] = university->groups[j + 1];
            }
            university->groupsCount--;
            if (university->groupsCount != 0) {
                university->groups = (Group *) exitRealloc(university->groups, university->groupsCount * sizeof(Group));
            }
            return true;
        }
    }
    university->groupsCount--;
    return true;
}

bool removeStudent(University *university, const unsigned long id) {
    if (university == NULL) {
        return false;
    }
    Student *student = getStudent(university, id);
    if (student == NULL) {
        return false;
    }
    Group *group = getGroup(university, student->groupName);
    for (unsigned int i = 0; i < group->studentsCount; i++) {
        if (group->students[i].id == id) {
            for (unsigned int j = i; j < group->studentsCount - 1; j++) {
                group->students[j] = group->students[j + 1];
            }
            group->studentsCount--;
            if (group->studentsCount != 0) {
                group->students = (Student *) exitRealloc(group->students, group->studentsCount * sizeof(Student));
            }
            return true;
        }
    }
    group->studentsCount--;
    return true;
}

Group *getGroup(const University *university, const char *name) {
    if (university == NULL) {
        return NULL;
    }
    for (unsigned int i = 0; i < university->groupsCount; i++) {
        if (strcmp(university->groups[i].name, name) == 0) {
            return &university->groups[i];
        }
    }
    return NULL;
}

Student *getStudent(const University *university, const unsigned long id) {
    if (university == NULL) {
        return NULL;
    }
    for (unsigned int i = 0; i < university->groupsCount; i++) {
        for (unsigned int j = 0; j < university->groups[i].studentsCount; j++) {
            if (university->groups[i].students[j].id == id) {
                return &university->groups[i].students[j];
            }
        }
    }
    return NULL;
}

void printUniversity(const University *university) {
    if (university == NULL) {
        return;
    }
    if (university->groupsCount == 0) {
        printf("University is empty\n");
        return;
    }
    for (unsigned group = 0; group < university->groupsCount; group++) {
        printf("-------------------------------------------------------------------------------\n");
        printGroup(university->groups[group]);
    }
    printf("-------------------------------------------------------------------------------\n");
}

void printGroup(const Group group) {
    if (group.studentsCount == 0) {
        printf("Group %s is empty\n", group.name);
        return;
    }
    printf("%s\n", group.name);
    for (unsigned student = 0; student < group.studentsCount; student++) {
        printStudent(group.students[student]);
    }
}

void printStudent(const Student student) {
    printf("\t%lu\n\t\t%s %s\n\t\t%u\n", student.id, student.surname, student.name, student.birthYear);
}

void freeUniversity(University *university) {
    if (university == NULL) {
        return;
    }
    for (unsigned int i = 0; i < university->groupsCount; i++) {
        free(university->groups[i].students);
    }
    free(university->groups);
    free(university);

}

bool saveToFile(const char *fileName, const University *university) {
    if (university == NULL) {
        return false;
    }
    FILE *file = fopen(fileName, "wb");
    if (file == NULL) {
        return false;
    }
    for (unsigned int i = 0; i < university->groupsCount; i++) {
        for (unsigned int j = 0; j < university->groups[i].studentsCount; j++) {
            fwrite(&university->groups[i].students[j], sizeof(Student), 1, file);
        }
    }
    fclose(file);
    return true;
}