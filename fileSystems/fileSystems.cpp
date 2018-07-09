#include<stdio.h>
#include<conio.h>
#include<stdbool.h>
struct user{
	char username[200];
	int category_offsets[5];
	char redundant_data[36];
};
struct user current_user;
struct category_node{
	char category_text[128];
	int message_offset[95];
	int next_message_node_offset;
};

struct message_node{
	int message_offset[127];
	int next_message_node_offset;
};


struct message{
	char text[128];
	int reply_offset[88];
	char username[32];
};

struct reply{
	char text[482];
	char username[30];
};
int zero_bit_pos(int a){
	int i = 0;
	while (a){
		if (!(0x80000000 & a)){
			return i;
		}
		else{
			a = a << 1;
		}
		i++;
	}
}
int find_free(int arr[], int len){
	int z = -1, first_zero = -1, i;
	for (i = 0; i < len; i++){
		if (arr[i] < 0xFFFFFFFF){
			z = zero_bit_pos(arr[i]);
			break;

		}
	}
	arr[i] |= 1 << (32 - z);
	return i;
}

int main(){
	FILE *fp = fopen("data.bin", "wb+");
	int bit_vector[512] = { 0 };
	struct user users = { "Peri", { 12, 0, 0, 0, 0 }, "kgfiubk" };
	if (fp != NULL)
	{
		printf("first ");
		fread(bit_vector, 4, 512, fp);
		/*for (int i=0; i < 512; i++){
		printf("%d", bit_vector[i]);
		}*/
		printf("read");
		printf("%x", ftell(fp));

		fwrite(&users, 256, 1, fp);
		printf("written");
		printf("%x", ftell(fp));
		struct user a;
		fseek(fp, -256, SEEK_CUR);
		printf("%x", ftell(fp));

		fread(&a, 256, 1, fp);
		printf("%s", a.username);

		printf("%d", a.redundant_data[0]);

	}
	else{
		printf("safdsg");
	}

	getch();
	return 0;
}
/*
int i;
	char a[10] = { "dinesh" }, c[100] = {"                                        "}, b[10];
	a[4] = 0;
	ptr = fopen("dineshBin.bin", "wb");
	fwrite(c, 1, 20, ptr);
	fclose(ptr);
	ptr = fopen("dineshBin.bin", "wb");
	fwrite(a, 4, 6, ptr);
	
	fclose(ptr);
	ptr = fopen("dineshBin.bin", "rb");
	fread(b, 4, 6, ptr);;
	for (i = 0; i < 6; i++)
		printf("%d", b[i]);
	getchar(); 
	fclose(ptr);

*/