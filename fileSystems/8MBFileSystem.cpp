#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>
#include<windows.h>
#include<io.h>
#define clear() system("cls")
#define	USERSTART 64
#define CATSTART 65
#define pf(n) printf("%d\n",n)
#define pfl() printf("\n\n\n\n************************************************************************************************\n\n\n");
typedef struct bitVector{
	char bits[8192];
}bitVector;
typedef struct message{
	char msg[128];
} message;


typedef struct messageOffset{
	int prev;
	int offsets[30];
	int next;
} messageOffset;


typedef struct userOffset{
	int prev;
	int offsets[30];
	int next;
} userOffset;


typedef struct catOffset{
	int prev;
	int offsets[30];
	int next;
} catOffset;


typedef struct category{
	char catName[62];
	char catDesc[62];
	int msgOffset;
} category;




typedef struct user{
	char userName[62];
	char passWord[62];
	int cat;								//Each user has 5 categories
} user;


void moveTo(FILE *fp, int off){
	fseek(fp, off, SEEK_SET);
}


int searchBitVector(FILE *fp){
	moveTo(fp, 0);
	bitVector b;
	fread(&b, sizeof(b), 1, fp);
	for (int i = 0; i < 65536; i++){
		if ((b.bits[i / 8] & (1 << (i % 8)))==0)
		{
			b.bits[i / 8] |= 1 << (i % 8);
			moveTo(fp, 0);
			fflush(stdin);fwrite(&b, sizeof(b), 1, fp);
			return i;
		}
	}
}


void password(char *s){	
	int i = 0;
	char ch;
	while ((ch = getch()) != '\r'){
		printf("*");
		s[i++] = ch;
	}
	s[i] = '\0';
}


int choice(){
	int ch;
	fflush(stdin);
	printf("\nEnter your choice:");
	scanf("%d", &ch);
	return ch;
}


void insertIntoUserOffset(FILE *fp, int off){
	moveTo(fp, USERSTART * 128);
	userOffset uo,newuo;
	memset(&newuo, 0, sizeof(userOffset));
	int i,newUserOff,forPrev=ftell(fp);
	while (1)
	{
		forPrev = ftell(fp);
		fread(&uo, sizeof(uo), 1, fp);
		if (uo.next == 0){
			if (uo.offsets[29] != 0){
				pf(ftell(fp));
				newUserOff = searchBitVector(fp);
				uo.next = newUserOff*128;
				pf(ftell(fp));
				moveTo(fp, forPrev);
				pf(ftell(fp));
				fflush(stdin);fwrite(&uo, sizeof(userOffset), 1, fp);
				moveTo(fp, newUserOff*128);
				newuo.prev = forPrev;
				newuo.offsets[0] = off;
				fflush(stdin);fwrite(&newuo, sizeof(userOffset), 1, fp);
				break;
			}
			else{
				for (i = 0; i < 30; i++)if (uo.offsets[i] == 0)break;
				uo.offsets[i] = off;
				fseek(fp, -128, SEEK_CUR);
				fflush(stdin);fwrite(&uo, sizeof(userOffset), 1, fp);
				break;
			}
		}
		else{
			moveTo(fp, uo.next);
		}
	}
}

void printUserNames(FILE *fp){
	moveTo(fp, USERSTART * 128);
	int i,flag=0,j=1;
	while (1){
		userOffset uo;
		user u;
		fread(&uo, sizeof(userOffset), 1, fp);
		for (i = 0; i < 30; i++){
			if (uo.offsets[i] == 0){
				flag++;
				break;
			}
			moveTo(fp, uo.offsets[i]*128);
			fread(&u, sizeof(user), 1, fp);
			printf("%d.%s\n", j, u.userName);
			j++;
		}
		if (flag)break;
		if (uo.next)
		moveTo(fp, uo.next);
	}
	if (j == 0)printf("There are no users to display....");
}

int searchForUser(FILE *fp,char *uname){
	moveTo(fp, USERSTART * 128);
	int i, flag = 0, j = 1;
	while (1){
		userOffset uo;
		user u;
		fread(&uo, sizeof(userOffset), 1, fp);
		for (i = 0; i < 30; i++){
			if (uo.offsets[i] == 0){
				flag++;
				break;
			}
			moveTo(fp, uo.offsets[i] * 128);
			fread(&u, sizeof(user), 1, fp);
			if (strcmp(u.userName, uname) == 0)return uo.offsets[i];
		}
		if (flag)break;
		if (uo.next)
			moveTo(fp, uo.next);
	}
	return 0;
}
void addReplys(FILE *fp, int mso){
	int m = searchBitVector(fp);
	moveTo(fp, m * 128);
	message ms;
	printf("Please Enter the message you want to enter:\n");
	gets(ms.msg);
	fwrite(&ms, sizeof(ms), 1, fp);
	moveTo(fp, mso * 128);
	messageOffset mo, newmo;
	memset(&newmo, 0, sizeof(messageOffset));
	int i, newMsgOff, forPrev = ftell(fp);
	while (1)
	{
		forPrev = ftell(fp);
		fread(&mo, sizeof(mo), 1, fp);
		if (mo.next == 0){
			if (mo.offsets[29] != 0){
				pf(ftell(fp));
				newMsgOff = searchBitVector(fp);
				mo.next = newMsgOff * 128;
				pf(ftell(fp));
				moveTo(fp, forPrev);
				pf(ftell(fp));
				fflush(stdin); fwrite(&mo, sizeof(messageOffset), 1, fp);
				moveTo(fp, newMsgOff * 128);
				newmo.prev = forPrev;
				newmo.offsets[0] = m;
				fflush(stdin); fwrite(&newmo, sizeof(messageOffset), 1, fp);
				break;
			}
			else{
				for (i = 0; i < 30; i++)if (mo.offsets[i] == 0)break;
				mo.offsets[i] = m;
				fseek(fp, -128, SEEK_CUR);
				fflush(stdin); fwrite(&mo, sizeof(catOffset), 1, fp);
				break;
			}
		}
		else{
			moveTo(fp, mo.next);
		}
	}
}
void addMessage(FILE *fp, int mso){
	int m=searchBitVector(fp);
	moveTo(fp, m*128);
	message ms;
	printf("Please Enter the message you want to enter:\n");
	gets(ms.msg);
	fwrite(&ms, sizeof(ms), 1, fp);
	moveTo(fp, mso*128);
	
	messageOffset mo, newmo;
	memset(&newmo, 0, sizeof(messageOffset));
	int i, newMsgOff, forPrev = ftell(fp);
	while (1)
	{
		forPrev = ftell(fp);
		fread(&mo, sizeof(mo), 1, fp);
		if (mo.next == 0){
			if (mo.offsets[29] != 0){
				pf(ftell(fp));
				newMsgOff = searchBitVector(fp);
				mo.next = newMsgOff * 128;
				pf(ftell(fp));
				moveTo(fp, forPrev);
				pf(ftell(fp));
				fflush(stdin); fwrite(&mo, sizeof(messageOffset), 1, fp);
				moveTo(fp, newMsgOff * 128);
				newmo.prev = forPrev;
				newmo.offsets[0] = m;
				fflush(stdin); fwrite(&newmo, sizeof(messageOffset), 1, fp);
				break;
			}
			else{
				for (i = 0; i < 30; i+=2)if (mo.offsets[i] == 0)break;
				mo.offsets[i] = m;
				fseek(fp, -128, SEEK_CUR);
				fflush(stdin); fwrite(&mo, sizeof(catOffset), 1, fp);
				break;
			}
		}
		else{
			moveTo(fp, mo.next);
		}
	}
}
/*void addReplys(FILE *fp,int mso){
	int re;
	messageOffset moff;
	memset(&moff, 0, sizeof(moff));
	moveTo(fp, mso * 128);



	int m = searchBitVector(fp);
	moveTo(fp, m * 128);
	message ms;
	printf("Please Enter the reply you want to enter:\n");
	gets(ms.msg);
	fwrite(&ms, sizeof(ms), 1, fp);
	moveTo(fp, mso * 128);

	messageOffset mo, newmo;
	memset(&newmo, 0, sizeof(messageOffset));
	int i, newMsgOff, forPrev = ftell(fp);
	while (1)
	{
		forPrev = ftell(fp);
		fread(&mo, sizeof(mo), 1, fp);
		if (mo.next == 0){
			if (mo.offsets[29] != 0){
				pf(ftell(fp));
				newMsgOff = searchBitVector(fp);
				mo.next = newMsgOff * 128;
				pf(ftell(fp));
				moveTo(fp, forPrev);
				pf(ftell(fp));
				fflush(stdin); fwrite(&mo, sizeof(messageOffset), 1, fp);
				moveTo(fp, newMsgOff * 128);
				newmo.prev = forPrev;
				newmo.offsets[0] = m;
				fflush(stdin); fwrite(&newmo, sizeof(messageOffset), 1, fp);
				break;
			}
			else{
				for (i = 0; i < 30; i += 2)if (mo.offsets[i] == 0)break;
				mo.offsets[i] = m;
				fseek(fp, -128, SEEK_CUR);
				fflush(stdin); fwrite(&mo, sizeof(catOffset), 1, fp);
				break;
			}
		}
		else{
			moveTo(fp, mo.next);
		}
	}
	
}*/
void viewAllReplys(FILE *fp, int mso){
	moveTo(fp, mso * 128);
	int i, flag = 0, j = 1;
	while (1){
		messageOffset mo;
		message m;
		fflush(stdin);
		int off = ftell(fp);
		fread(&mo, sizeof(mo), 1, fp);
		for (i = 0; i < 30; i += 2){

			if (mo.offsets[i] == 0){
				flag++;
				break;
			}
			if (mo.offsets[i] != 1){
				moveTo(fp, mo.offsets[i] * 128);
				fread(&m, sizeof(message), 1, fp);
				printf("%d.%s\n", j, m.msg);
				j++;
			}
		}
		if (j != 1){
		choice:
			pfl();
			printf("1.Delete reply\n2.Exit\n");
			pfl();
			int ch = choice();
			int num;

			if (ch == 1){
				printf("Enter the message number:");
				scanf("%d", &num);
				int msgoff, repoff, present = ftell(fp);
				msgoff = mo.offsets[2 * (num - 1)];
				repoff = mo.offsets[(2 * (num - 1)) + 1];
				mo.offsets[2 * (num - 1)] = 1;
				mo.offsets[(2 * (num - 1)) + 1] = 1;
				moveTo(fp, 0);
				bitVector b;
				fread(&b, sizeof(b), 1, fp);
				if(msgoff)b.bits[(msgoff / 8)] &= ~(1 << (msgoff % 8));
				if(repoff)b.bits[(repoff / 8)] &= ~(1 << (repoff % 8));
				moveTo(fp, 0);
				fflush(stdin);
				fwrite(&b, sizeof(b), 1, fp);
				moveTo(fp, off);
				fflush(stdin);
				fwrite(&mo, sizeof(mo), 1, fp);
				printf("\n\n\nMessage Deleted successfully...");
				getch();
			}
			else if (ch == 2){
				//doing nothing...
			}
			else{
				printf("Please enter a correct choice...\n");
				goto choice;
			}
		}
		if (flag)break;
		if (mo.next)
			moveTo(fp, mo.next);
	}
	if (j == 1)printf("There are no Messages to display....");
}

void viewAllMessages(FILE *fp, int mso){
	moveTo(fp, mso * 128);
	clear();
	int i, flag = 0, j = 1;
	while (1){
		messageOffset mo;
		message m;
		fflush(stdin);
		int off = ftell(fp);
		fread(&mo, sizeof(mo), 1, fp);
		for (i = 0; i < 30; i+=2){
			
			if (mo.offsets[i] == 0){
				flag++;
				break;
			}
			if (mo.offsets[i] != 1){
				moveTo(fp, mo.offsets[i] * 128);
				fread(&m, sizeof(message), 1, fp);
				printf("%d.%s\n", j, m.msg);
				j++;
			}
		}
		if (j != 1){
		choice:
			pfl();
			printf("1.Delete Message\n2.Add replys to a Message\n3.View replys of a Message\n4.Exit\n");
			pfl();
			int ch = choice();
			int num;

			if (ch == 1){
				printf("Enter the message number:");
				scanf("%d", &num);
				int msgoff, repoff, present = ftell(fp);
				msgoff = mo.offsets[2 * (num - 1)];
				repoff = mo.offsets[(2 * (num - 1)) + 1];
				mo.offsets[2 * (num - 1)] = 1;
				mo.offsets[(2 * (num - 1)) + 1] = 1;
				moveTo(fp, 0);
				bitVector b;
				fread(&b, sizeof(b), 1, fp);
				if (msgoff)b.bits[(msgoff / 8)] &= ~(1 << (msgoff % 8));
				if(repoff)b.bits[(repoff / 8)] &= ~(1 << (repoff % 8));
				moveTo(fp, 0);
				fflush(stdin);
				fwrite(&b, sizeof(b), 1, fp);
				moveTo(fp, off);
				fflush(stdin);
				fwrite(&mo, sizeof(mo), 1, fp);
				printf("\n\n\nMessage Deleted successfully...");
				getch();
			}
			else if (ch == 2){
				printf("Enter the message number:");
				scanf("%d", &num);
				if (mo.offsets[(2 * (num - 1)) + 1] == 0){
					mo.offsets[(2 * (num - 1)) + 1] = searchBitVector(fp);
					moveTo(fp, mo.offsets[(2 * (num - 1)) + 1]);
					messageOffset tempmo;
					memset(&tempmo, 0, sizeof(tempmo));
					fwrite(&tempmo, sizeof(tempmo), 1, fp);
					moveTo(fp, off);
					fwrite(&mo, sizeof(mo), 1, fp);
				}
				addReplys(fp, mo.offsets[(2 * (num - 1)) + 1]);
			}
			else if (ch == 3){
				printf("Enter the message number:");
				scanf("%d", &num);
				if (mo.offsets[(2 * (num - 1)) + 1] != 0)
					viewAllReplys(fp, mo.offsets[(2 * (num - 1)) + 1]);
				else printf("\n\n\nThere are no replys for this message...");
			}
			else if (ch == 4){
				//doing nothing...
			}
			else{
				printf("Please enter a correct choice...\n");
				goto choice;
			}
		}
		if (flag)break;
		if (mo.next)
			moveTo(fp, mo.next);
	}
	if (j == 1)printf("There are no Messages to display....");
}
void selectCategoryNumber(FILE *fp, int co){
choice:
	clear();
	pfl();
	printf("1.Add Message\n2.View All messages\n3.Exit");
	pfl();
	int ch = choice();
	category c;
	messageOffset mo;
	message m;
	moveTo(fp, co * 128);
	fread(&c, sizeof(c), 1, fp);
	if (c.msgOffset == 0){
		c.msgOffset = searchBitVector(fp);
		moveTo(fp, co * 128);
		fflush(stdin);
		fwrite(&c, sizeof(c), 1, fp);
		moveTo(fp, c.msgOffset);
		memset(&mo, 0, sizeof(mo));
		fwrite(&mo, sizeof(mo), 1, fp);
	}
	if (ch == 1){
		
		addMessage(fp, c.msgOffset);
		getch();
		goto choice;
	}
	else if (ch == 2){
		viewAllMessages(fp, c.msgOffset);
		getch();
		goto choice;
	}
	else if (ch == 3){
		return;
	}
	else {
		printf("Please Enter correct choice!!!");
		goto choice;
	}
}
void viewMyCategories(FILE *fp, int co){
	user u;
	moveTo(fp, co * 128);
	int i, flag = 0, j = 1;
	clear();
	while (1){
		catOffset co;
		category c;
		fread(&co, sizeof(catOffset), 1, fp);
		for (i = 0; i < 30; i++){
			if (((i + 1) % 10 == 0)||co.offsets[i]==0){
			choice:
				pfl();
				printf("\n1.Next\n2.Select Category\n3.Exit\n");
				pfl();
				int ch = choice();
				if (ch == 1){
					clear();
				}
				else if (ch == 2){
					int num;
					printf("Enter Category Number:");
					scanf("%d", &num);
					selectCategoryNumber(fp, co.offsets[num - 1]);
				}
				else if (ch == 3){
					//do nothin....
				}
				else {
					printf("Please enter correct choice!!!!");
					clear();
					goto choice;
				}
			}
			if (co.offsets[i] == 0){
				flag++;
				break;
			}
			moveTo(fp, co.offsets[i] * 128);
			memset(&c, 0, sizeof(c));
			fread(&c, sizeof(category), 1, fp);
			printf("\n%d.%s\nDescription:%s", i + 1, c.catName, c.catDesc);
			j++;
		}
		if (flag)break;
		if (co.next)
			moveTo(fp, co.next);
	}
	if (j == 1)printf("There are no categories to display....");
}
void viewAllCategories(FILE *fp){
	moveTo(fp, CATSTART*128);
	user u;
	int i, flag = 0, j = 1;
	while (1){
		catOffset co;
		category c;
		fread(&co, sizeof(catOffset), 1, fp);
		for (i = 0; i < 30; i++){
			if (co.offsets[i] == 0){
				flag++;
				break;
			}
			viewMyCategories(fp, co.offsets[i]);
			j++;
		}
		if (flag)break;
		if (co.next)
			moveTo(fp, co.next);
	}
	if (j == 1)printf("There are no categories to display....");
}
void addToAllCats(FILE *fp, int catOff){
	moveTo(fp, CATSTART * 128);
	catOffset co, newco;
	memset(&newco, 0, sizeof(catOffset));
	int i, newCatOff, forPrev = ftell(fp);
	while (1)
	{
		forPrev = ftell(fp);
		fread(&co, sizeof(co), 1, fp);
		if (co.next == 0){
			if (co.offsets[29] != 0){
				pf(ftell(fp));
				newCatOff = searchBitVector(fp);
				co.next = newCatOff * 128;
				pf(ftell(fp));
				moveTo(fp, forPrev);
				pf(ftell(fp));
				fflush(stdin);fwrite(&co, sizeof(catOffset), 1, fp);
				moveTo(fp, newCatOff * 128);
				newco.prev = forPrev;
				newco.offsets[0] = catOff;
				fflush(stdin);fwrite(&newco, sizeof(catOffset), 1, fp);
				break;
			}
			else{
				for (i = 0; i < 30; i++)if (co.offsets[i] == 0)break;
				co.offsets[i] = catOff;
				fseek(fp, -128, SEEK_CUR);
				fflush(stdin);fwrite(&co, sizeof(catOffset), 1, fp);
				break;
			}
		}
		else{
			moveTo(fp, co.next);
		}
	}
}
void addCategories(FILE *fp, user u,int uo,category c){
	//hicatOffset c;
	catOffset co, newco;
	if (u.cat == 0){
		u.cat = searchBitVector(fp);
		moveTo(fp, uo*128);
		fflush(stdin);fwrite(&u, sizeof(u), 1, fp);
		addToAllCats(fp, u.cat);
		memset(&co, 0, sizeof(catOffset));
		co.offsets[0] = searchBitVector(fp);
		moveTo(fp, co.offsets[0]*128);
		fflush(stdin); fwrite(&c, sizeof(c), 1, fp);
		moveTo(fp,u.cat*128);
		fflush(stdin);fwrite(&co, sizeof(catOffset), 1, fp);
	}
	else{
		int cato = searchBitVector(fp);
		moveTo(fp, cato * 128);
		fflush(stdin);
		fwrite(&c, sizeof(category), 1, fp);
		moveTo(fp, u.cat * 128);

		memset(&newco, 0, sizeof(catOffset));
		int i, newCatOff, forPrev = ftell(fp);
		while (1)
		{
			forPrev = ftell(fp);
			fread(&co, sizeof(co), 1, fp);
			if (co.next == 0){
				if (co.offsets[29] != 0){
					pf(ftell(fp));
					newCatOff = searchBitVector(fp);
					co.next = newCatOff * 128;
					pf(ftell(fp));
					moveTo(fp, forPrev);
					pf(ftell(fp));
					fflush(stdin); fwrite(&co, sizeof(catOffset), 1, fp);
					moveTo(fp, newCatOff * 128);
					newco.prev = forPrev;
					newco.offsets[0] = cato;
					fflush(stdin); fwrite(&newco, sizeof(catOffset), 1, fp);
					break;
				}
				else{
					for (i = 0; i < 30; i++)if (co.offsets[i] == 0)break;
					co.offsets[i] = cato;
					pf(forPrev);
					pf(ftell(fp));
					fseek(fp, -128, SEEK_CUR);
					pf(ftell(fp));
					fflush(stdin);
					fflush(stdin); fwrite(&co, sizeof(catOffset), 1, fp);
					break;
				}
			}
			else{
				moveTo(fp, co.next);
			}
		}
	}
}

void showUserMenu(FILE *fp,user u,int uo){
	category c;
	memset(&c, 0, sizeof(c));
choice:
	clear();
	pfl();
	printf("1.Add Categories\n");
	printf("2.View your categories\n");
	printf("3.View All Categories\n");
	printf("4.Logout\n");
	pfl();
	int ch = choice();
	if (ch == 1){
		fflush(stdin);
		
		printf("Enter Category Name:");
		gets(c.catName);
		fflush(stdin);
		printf("Enter category Description:");
		gets(c.catDesc);
		addCategories(fp, u,uo,c);
		goto choice;
	}
	else if (ch == 2){
		moveTo(fp, uo * 128);
		fread(&u, sizeof(u), 1, fp);
		if (u.cat ==0)
			printf("Please add any categories...");
		else{
			viewMyCategories(fp, u.cat);
		}
		getch();
		goto choice;
	}
	else if (ch == 3){
		viewAllCategories(fp);
		getch();
		goto choice;
	}
	else if (ch == 4){
		return;
	}
	else{
		printf("\n\t\t\t\t\t\t\tWrong Choice!!!!");
		getch();
		goto choice;
	}
}


void showMainMenu(FILE *fp){
	int ch,userOffset,flag;		
	user u;
	char uname[62], pwd[62];
	int j = 0;
start:
	clear();
	pfl();
	printf("1.Login to your account\n");
	printf("2.SignUp for a new account\n");
	printf("3.Show all users\n");
	printf("4.Exit\n");
	pfl();
	ch=choice();
	if (ch==1){
	login:
		clear();
		memset(uname, 0, 54);
		memset(pwd, 0, 54);
		printf("Enter Username:");
		scanf("%s",&uname);
		printf("Enter Password");
		password(pwd);
		userOffset=searchForUser(fp,uname);
		if (userOffset != 0){
			moveTo(fp, userOffset*128);
			fread(&u,sizeof(u),1,fp);
			flag = strcmp(u.passWord, pwd);
		}
		if (userOffset == 0 || flag!=0){
			printf("Invalid credentials...Please enter the correct information...");
			getch();
			goto login;
		}
		else if (flag == 0){
			clear();
			printf("\n\t\t\t\t\t\t\tWelcome %s...\n\n\n\n", u.userName);
			showUserMenu(fp,u,userOffset);
		}
		goto start;
	}
	else if (ch == 2){
	scan:
		clear();
		memset(&u, 0, sizeof(user));
		printf("Choose Username:");
		scanf("%s", u.userName);
		if (searchForUser(fp, u.userName)){
			printf("Username already exists...Please try with another user name...");
			getch();
			goto scan;
		}
		printf("Choose Password:");
		password(u.passWord);
		int offset = searchBitVector(fp);
		insertIntoUserOffset(fp, offset);
		moveTo(fp, offset*128);
		fflush(stdin);fwrite(&u, sizeof(user), 1, fp);
		printf("\n\n\t\t\t Registration Completed Successfully... ");
		getch();
		goto start;
	}
	else if (ch == 3){
		clear();
		printf("List of userNames:\n");
		printUserNames(fp);
		getch();
		goto start;
	}
	else if (ch == 4){

		printf("Thank You for using our services...\n\n");
		return;
	}
	else{
		printf("Wrong choice!!! Try again\n");
		goto start;
	}
}
void initialize(FILE *fp){
	bitVector b;
	userOffset u;
	catOffset c;
	moveTo(fp, 8388606);
	char ch = fgetc(fp);
	if (ch == NULL){
		printf("entered null");
		fseek(fp, -1, SEEK_CUR);
		fputc('1',fp);
		moveTo(fp, 0);
		memset(&b, 0, sizeof(bitVector));
		for (int i = 0; i < 66;i++)
			b.bits[i/8] |= 1 << (i % 8);
		fflush(stdin);fwrite(&b, sizeof(b), 1, fp);
		memset(&u, 0, sizeof(u));
		fflush(stdin);fwrite(&u, sizeof(u), 1, fp);
		memset(&c, 0, sizeof(c));
		fflush(stdin);fwrite(&c, sizeof(c), 1, fp);
	}


}
int main(){
	FILE *fp;
	if (_access("fs.bin", 0) == -1)
		system("fsutil file createnew fs.bin 8388608");
	fp = fopen("fs.bin", "rb+");
	initialize(fp);
	showMainMenu(fp);
	fclose(fp);
	getch();
	return 0;
}