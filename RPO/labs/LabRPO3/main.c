#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lab3.h"

typedef enum {
    E_INVALID = 0,
    E_ADD_GROUP,
    E_ADD_STUDENT,
    E_DEL_GROUP,
    E_DEL_STUDENT,
    E_PRINT_UNI,
    E_PRINT_GROUP,
    E_PRINT_STUDENT,
    E_END
} EAction;

const char *Enum2Str(EAction eAct) {
    switch (eAct) {
        case E_INVALID:
            return " - Invalid command";
        case E_ADD_GROUP:
            return " - Add a group";
        case E_ADD_STUDENT:
            return " - Add a student";
        case E_DEL_GROUP:
            return " - Remove a group";
        case E_DEL_STUDENT:
            return " - Remove a student";
        case E_PRINT_UNI:
            return " - Print information about the university";
        case E_PRINT_GROUP:
            return " - Print information about the group";
        case E_PRINT_STUDENT:
            return " - Print information about the student";
        case E_END:
            return " - End of work";
        default:
            return " ? - Invalid command";
    }
}

int main(int argc, char *argv[]) {
    // Проверка аргументов командной строки, если не заданы - ввод с клавиатуры
    char *filename = (char *) malloc(SIZE);
    if (argc == 2) {
        strcpy(filename, argv[1]);
    } else {
        printf("Usage: %s <filename>\n", argv[0]);
        printf("Enter data file path: ");
        scanf("%s", filename);
    }

    // Инициализация университета файлом с заданным путем
    University *uni = initUniversity(filename);
    char *input = (char *) malloc(SIZE);
    EAction eAction = E_INVALID;
    while (eAction != E_END) {
        // вывод списка всех возможных команд - код команды и текст из Enum2Str
        printf("\n");
        for (int i = 1; i <= E_END; i++) {
            printf("%d%s\n", i, Enum2Str((EAction) i));
        }
        // ввод кода команды пользователем
        printf("Enter command: ");
        scanf("%s", input);
        eAction = (EAction) atoi(input);

        switch (eAction) {
            // обработка каждой команды и действие по ней
            case E_ADD_GROUP: {
                printf("Enter group name: ");
                char *groupName = (char *) malloc(SIZE);
                scanf("%s", groupName);
                Group group = {{0}, 0, 0};
                strcpy(group.name, groupName);
                group.students = (Student *) malloc(sizeof(Student));
                addNewGroup(uni, group);
                free(groupName);
                break;
            }

            case E_ADD_STUDENT: {
                printf("Enter group name: ");
                char *groupName = (char *) malloc(SIZE);
                scanf("%s", groupName);
                printf("Create a student\nNAME SURNAME YEAR: ");
                char *name = (char *) malloc(SIZE);
                char *surname = (char *) malloc(SIZE);
                int year = 2007;
                scanf("%s %s %d", name, surname, &year);
                Student student = {0, {0}, {0}, {0}, 0};
                strcpy(student.name, name);
                strcpy(student.surname, surname);
                strcpy(student.groupName, groupName);
                addNewStudent(getGroup(uni, groupName), student);
                free(groupName);
                free(name);
                free(surname);
                break;
            }

            case E_DEL_GROUP: {
                printf("Enter group name: ");
                char *groupName = (char *) malloc(SIZE);
                scanf("%s", groupName);
                removeGroup(uni, groupName);
                free(groupName);
                break;
            }

            case E_DEL_STUDENT: {
                printf("Enter student id: ");
                int id = -1;
                scanf("%d", &id);
                if (id == -1) {
                    printf("Invalid id\n");
                    break;
                }
                removeStudent(uni, id);
                break;
            }

            case E_PRINT_UNI: {
                printUniversity(uni);
                break;
            }

            case E_PRINT_GROUP: {
                printf("Enter group name: ");
                char *groupName = (char *) malloc(SIZE);
                scanf("%s", groupName);
                Group *group = getGroup(uni, groupName);
                if (group == NULL) {
                    printf("Group not found\n");
                } else {
                    printGroup(*group);
                }
                free(groupName);
                break;
            }

            case E_PRINT_STUDENT: {
                printf("Enter sudent id: ");
                int id;
                scanf("%d", &id);
                Student *student = getStudent(uni, id);
                if (student == NULL) {
                    printf("Student not found\n");
                } else {
                    printStudent(*student);
                }
                break;
            }

            case E_END: {
                break;
            }

            case E_INVALID: {
                printf("Invalid command\n");
                break;
            }

            default: {
                printf("Absolutely invalid command\n");
                break;
            }
        }
    }

    // сохранение данных в файл
    saveToFile(filename, uni);
    freeUniversity(uni);
    free(input);
    free(filename);
    return 0;
}