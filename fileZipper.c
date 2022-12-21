#include<zip.h>
#include<string.h>
#include<stdlib.h>

int check(struct zip *zipPointer, int *err) {
    if(zipPointer==NULL) {
        printf("Zip file opening/creation failed !!!\nError code: %d\n", *err);
        return -1;
    }
}

int showContent(struct zip *zipPointer, int numEntries, struct zip_stat zipStat) {

    numEntries = zip_get_num_entries(zipPointer, 0);
    if(numEntries>0) {
        printf("Contents of Zip File are:\n");
        for(int i=0; i<numEntries; ++i) {
            zip_stat(zipPointer, zip_get_name(zipPointer, i, 0), 0, &zipStat);
            printf("%s %d\n", zipStat.name, zipStat.size);
        }
    }
    else {
        printf("Empty Zip File !!!\n");
        return -1;
    }

    zip_close(zipPointer);
}

int extractFile(struct zip *zipPointer, int numEntries, struct zip_stat zipStat){
    if(numEntries>0) {
        printf("Extracting Files.....\n");
        for(int i=0; i<numEntries; ++i) {
            zip_stat(zipPointer, zip_get_name(zipPointer, i, 0), 0, &zipStat);
            char *contents = malloc(zipStat.size);
            struct zip_file *filePointer = zip_fopen(zipPointer, zipStat.name, 0);
            if(filePointer==NULL) {
                printf("Can't read Zip file contents !!!\n");
                return -1;
            }
            zip_fread(filePointer, contents, zipStat.size);
            FILE *extractedFile = fopen(zipStat.name, "wb");
            if(extractedFile==NULL) {
                printf("Can't create extracted file !!!\n");
                return -1;
            }
            fwrite(contents, zipStat.size, 1, extractedFile);
            fclose(extractedFile);
            zip_fclose(filePointer);
        }
    }
    else {
        printf("Empty Zip File!!\n");
        return -1;
    }

    zip_close(zipPointer);
}

void zipFile(struct zip *zipPointer, char *fileName) {
    zip_source_t *source = zip_source_file(zipPointer, fileName, 0, -1);
    zip_file_add(zipPointer, fileName, source, ZIP_FL_ENC_UTF_8);
    zip_close(zipPointer);
    // zip_source_free(source);
}

int main(int argc, char *argv[]) {
    struct zip *zipPointer = NULL;
    int err = 0;
    int numEntries = 0;

    if(argc<3 || argc>3) {
        printf("Run Program with file e.g(unzip -[options] [arguments])\nOptions:\n-1 :View File Contents e.g. unzip -1 zipfile.zip\n-2 :Extract File Contents e.g. unzip -2 zipfile.zip\n-3 :Zip Folder e.g. unzip -3 foldername");
        return -1;
    }

    if(strcmp(argv[1], "-3")==0) {
        char zipName[strlen(argv[2]) + 5];
        strcpy(zipName, argv[2]);
        strcat(zipName, ".zip");
        
        zipPointer = zip_open(zipName, ZIP_CREATE, &err);
        check(zipPointer, &err);
        zipFile(zipPointer, argv[2]);
    }
    else{
        zipPointer = zip_open(argv[2], 0, &err);
        check(zipPointer, &err);
        
        struct zip_stat zipStat;
        zip_stat_init(&zipStat);

        numEntries = zip_get_num_entries(zipPointer, 0);

        if(strcmp(argv[1], "-1") == 0) {
        showContent(zipPointer, numEntries, zipStat);
        }
        if(strcmp(argv[1], "-2") == 0) {
            extractFile(zipPointer, numEntries, zipStat);
        }
    }
    return 0;
}