#include "headers/getTraits.h"
#include "headers/ntree.h"
#include "headers/str.h"
#include <stdio.h>
#include <stdlib.h>

#define AUTOSOMAL 8
#define XLINKED 6

static void getLine(Str* line)
{
    int c;
    while ((c = getchar()) != EOF && c != '\n') StrAppend(line, c);
}

bool ask(const char* question) {
    Str input; StrNew(&input);
    bool answer, validInput = false;

    while (!validInput) {
        system("clear");
        printf("%s (Y/N):\n", question);
        getLine(&input);
        switch (input.data[0]) {
            case 'y': case 'Y':
                answer = true;
                validInput = true;
                continue;
            case 'n': case 'N':
                answer = false;
                validInput = true;
                continue;
        }
        input.len = 0;
        printf("Invalid input. Chose from Y or N (case insensitive)\nPress ENTER to continue\n");
        getLine(&input);
        input.len = 0;
    }
    
    free(input.data);
    return answer;
}

Traits* getTraits()
{
    Traits outline; TraitsNew(&outline);

    bool shouldKeepGettingTraits = true;
    while (shouldKeepGettingTraits)
    {
        bool isAutosomal = ask("Is the trait autosomal(Y) or sex-linked(N)?");

        char* dominantPhenotype, *recessivePhenotype;
        Str input; StrNew(&input);

        printf("What is the phenotype of the dominant allele?:\n");
        getLine(&input);
        dominantPhenotype = input.data;
        StrNew(&input);

        printf("What is the phenotype of the recessive allele?:\n");
        getLine(&input);
        recessivePhenotype = input.data;

        Trait trait;
        TraitFrom(&trait, dominantPhenotype, recessivePhenotype, isAutosomal ? AUTOSOMAL : XLINKED, -1);

        TraitsAppend(&outline, trait, isAutosomal);

        shouldKeepGettingTraits = ask("Do you want another trait?");
    }

    const unsigned MOM = 0;
    const unsigned DAD = 1;

    Traits* traits = malloc(sizeof(Traits) * 2);
    TraitsNew(&traits[MOM]);
    TraitsNew(&traits[DAD]);

    for (int i = 0; i < outline.lenAuto; ++i)
    {
        int genotype[2];
        printf("Is the mom's phenotype %s(Y) or %s(N)?", outline.autosomal[i].dominant, outline.autosomal[i].recessive);
        bool isDominant = ask("");
        if (isDominant) {
            genotype[0] = DOMINANT;
            bool heterozygous = ask("Is the genotype heterozygous(Y) or homozygous(N)?");
            genotype[1] = heterozygous ? RECESSIVE : DOMINANT;
        }
        else {
            genotype[0] = RECESSIVE;
            genotype[1] = RECESSIVE;
        }
        Trait trait; TraitFrom(&trait, outline.autosomal[i].dominant, outline.autosomal[i].recessive, genotype[0], genotype[1]);
        TraitsAppend(&traits[MOM], trait, true);
    }

    for (int i = 0; i < outline.lenAuto; ++i)
    {
        int genotype[2];
        printf("Is the dad's phenotype %s(Y) or %s(N)?", outline.autosomal[i].dominant, outline.autosomal[i].recessive);
        bool isDominant = ask("");
        if (isDominant) {
            genotype[0] = DOMINANT;
            bool heterozygous = ask("Is the genotype heterozygous(Y) or homozygous(N)?");
            genotype[1] = heterozygous ? RECESSIVE : DOMINANT;
        }
        else {
            genotype[0] = RECESSIVE;
            genotype[1] = RECESSIVE;
        }
        Trait trait; TraitFrom(&trait, outline.autosomal[i].dominant, outline.autosomal[i].recessive, genotype[0], genotype[1]);
        TraitsAppend(&traits[DAD], trait, true);
    }

    for (int i = 0; i < outline.lenX; ++i)
    {
        int genotype[2];
        printf("Is the mom's phenotype %s(Y) or %s(N)?", outline.xLinked[i].dominant, outline.xLinked[i].recessive);
        bool isDominant = ask("");
        if (isDominant) {
            genotype[0] = DOMINANT_X;
            bool heterozygous = ask("Is the genotype heterozygous(Y) or homozygous(N)?");
            genotype[1] = heterozygous ? RECESSIVE_X: DOMINANT_X;
        }
        else {
            genotype[0] = RECESSIVE_X;
            genotype[1] = RECESSIVE_X;
        }
        Trait trait; TraitFrom(&trait, outline.xLinked[i].dominant, outline.xLinked[i].recessive, genotype[0], genotype[1]);
        TraitsAppend(&traits[MOM], trait, false);
    }

    for (int i = 0; i < outline.lenX; ++i)
    {
        int genotype[2];
        printf("Is the dad's phenotype %s(Y) or %s(N)?", outline.xLinked[i].dominant, outline.xLinked[i].recessive);
        bool isDominant = ask("");
        if (isDominant) {
            genotype[1] = DOMINANT_X;
            genotype[0] = Y;
        }
        else {
            genotype[0] = RECESSIVE_X;
            genotype[1] = Y;
        }
        Trait trait; TraitFrom(&trait, outline.xLinked[i].dominant, outline.xLinked[i].recessive, genotype[0], genotype[1]);
        TraitsAppend(&traits[DAD], trait, false);
    }
    return traits;
}
