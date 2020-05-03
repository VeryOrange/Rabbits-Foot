#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <pwd.h>
#include <unistd.h>



void decryptfile(FILE * fpin,FILE* fpout,unsigned char* key, unsigned char* iv);
void ls_dir(char* start_path);
char * get_home_dir();

void main()
{

       
	char *home = get_home_dir();
	printf("%s\n",home);
        ls_dir(home);
}

void ls_dir(char* path)
{
	char newPath[1000];
        unsigned char key[] = "12345678901234561234567890123456"; //32 chars long
        unsigned char iv[] = "1234567890123456";//16 chars long
	DIR* direc;
        struct dirent *entry;
	if((direc=opendir(path)) ==NULL)
	{
		printf("Empty dir\n");
		return;
	}
	while((entry=readdir(direc)) !=NULL)
	{
	   if(strcmp(entry->d_name,".") !=0 && strcmp(entry->d_name,"..")!=0){
		printf("%d\n",entry->d_type);
		printf("%s\n",entry->d_name);
		if(entry->d_type ==4)
		{
			printf("Directory %s\n", path);
			strcpy(newPath,path);
			strcat(newPath,"/");
			strcat(newPath,entry->d_name);
			
			if(newPath != path && entry->d_name[0] != '.')
			{	
				printf("%s\n",newPath);
				ls_dir(newPath);
			}
		}

		if(entry->d_type == 8)
		{	
			printf("Is a file\n");

			int len = strlen(entry->d_name);
			const char* suff = &entry->d_name[len-9];
			printf("%s\n",suff);
			if(strcmp(suff,".blackcat") == 0)
			{

				char* full_path =(char*) malloc(strlen(entry->d_name)+strlen(path)+2);
				strcpy(full_path,path);
				strcat(full_path,"/");
				strcat(full_path,entry->d_name);
				char* new_name = (char*) malloc(strlen(full_path)+1);
				strcpy(new_name,full_path);
				new_name[strlen(new_name)-9] = '\0';
				printf("%s\n",new_name);

				FILE* fpin;
				FILE* fpout;
				printf("%s\n",full_path);

				fpin=fopen(full_path,"rb");
				fpout=fopen(new_name,"wb");
				if(fpin == NULL){
					break;
				}

				decryptfile(fpin,fpout,key,iv);
				if(fpin != NULL)
					fclose(fpin);
				if(fpout != NULL)
					fclose(fpout);
					
				remove(full_path);
				free(full_path);
				free(new_name);


			}
	

		}
	   }

	}

}



void decryptfile(FILE * fpin,FILE* fpout,unsigned char* key, unsigned char* iv)
{
	//Using openssl EVP to encrypt a file


	const unsigned bufsize = 4096; // bytes to read
	unsigned char* read_buf = malloc(bufsize); // buffer to hold file text
	unsigned char* cipher_buf ;// decrypted text
	unsigned blocksize;
	int out_len;

	EVP_CIPHER_CTX *ctx;
	ctx = EVP_CIPHER_CTX_new();

	EVP_CipherInit(ctx,EVP_aes_256_cbc(),key,iv,0); // 0 = decrypt 	1= encrypt
	blocksize = EVP_CIPHER_CTX_block_size(ctx);
	cipher_buf = malloc(bufsize+blocksize);

	// read file and write encrypted file until eof
	while(1)
	{
		if(fpin == NULL){
			break;
		}
		int bytes_read = fread(read_buf,sizeof(unsigned char),bufsize,fpin);
		EVP_CipherUpdate(ctx,cipher_buf,&out_len,read_buf, bytes_read);
		fwrite(cipher_buf,sizeof(unsigned char),out_len,fpout);
		if(bytes_read < bufsize)
		{
			break;//EOF
		}
	}

	EVP_CipherFinal(ctx,cipher_buf,&out_len);
	fwrite(cipher_buf,sizeof(unsigned char),out_len,fpout);

	free(cipher_buf);
	free(read_buf);
}

char * get_home_dir(){
	uid_t uid = getuid();
	struct passwd *pw = getpwuid(uid);
	
	if(pw == NULL){
		return 0;
	}

	return (pw->pw_dir);

}
