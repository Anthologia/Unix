// "fio_bin_fwrite.c"�� ������ include�� ��� �ѹ��� include�ϱ� ����
#ifndef _FILE_IO_C
#define _FILE_IO_C

//*****************************************************************************
//   FILE ���� �ݱ� �Լ���:  C ǥ�� fopen() fclose() fsetpos()
//*****************************************************************************

static FILE *fp;

int r_open_fp(char *name) { // ���� �ҷ����� : �б������ ����
	return( (fp = fopen(name, "r")) ? 0 : -1 );
	// �� ������ return ( (fp = fopen(name, "r")) != NULL ? 0 : -1); �� ������
	// NULL DMS (void *)0, �� ������ ��(�ּ� ��)�� 0��
 	// FILE *P = NULL; �̰��� p = (void *)0�� ����, p���� ��� 0�� �����
	// if (p)�ϸ�, �� ���ǹ��� p�� 0�̹Ƿ� FALSE��
  	// ���� p�� NULL�� �ƴϸ� TRUE��
}

// �Ʒ� �Լ����� FILE I/O type 0, 1, 2���� ��� ����
int w_open_fp(char *name) {
	return( (fp = fopen(name, "w")) ? 0 : -1 );
}

int n_open_fp(char *name) { // �ٸ� (��) �̸����� �����ϱ� : ��������� ����
	//���� �б������ ���� ����
	if((fp = fopen(name, "r"))) {
	fclose(fp);
	return (-2);
	}
	return( (fp = fopen(name, "w")) ? 0 : -1 );
}

int rw_open_fp(char *name) { // ������ �б�/��������� ���� (���� ������ ����)
						 // ���� ���ڵ� ���� ���� �޴� ���� �� ȣ���
	return( (fp = fopen(name, "r+")) ? 0 : -1 );
}

int rwc_open_fp(char *name) { // ������ �б�, ��������� ���� (���� ������ ����)
	return((fp = fopen(name, "w+")) ? 0 : -1);
}

//	DB ������ �ݴ´�.
int close_fp(void) { // fp ���� �ݱ�
	return(fclose(fp));
}

int setpos_fp(int pos) { // fp ������ �а� ���� ��ġ(file position)�� pos�� �ű�
	return(fseek(fp, pos, SEEK_SET)); // SEEK_SET, SEEK_END, SEEK_CUR
}

//===========================================================================
// TEXT FILE I/O ���� �Լ��� : C ǥ�� fscanf() fprintf()
//===========================================================================

static void
trunc_space(char *p)
{
	while (*p) ++p;
	for (--p; isspace((int)*p); --p) ;
	*(++p) = 0;
}

//	DB ���Ͽ��� ���ڵ� �ϳ��� �о� �޸� r�� �����Ѵ�.
int
fscanf_rec(rec_t *r) {
	if(fscanf(fp, "%d %s %s %d", &r->key, r->name, r->dep, &r->grade) <= 0)
		return 0;

	fgetc(fp); // �ּ� �տ� �ִ� ' ' �о� ����
			   // �ּҴ� "%s"�� �о� ���� �� ����(�ܾ ���� ����)

	if (fgets(r->addr, 40, fp) == NULL)
		return 0; // ���� ���� 40�� : %-38s + '\n' + '\0'

	trunc_space(r->addr); // �ּ� ���� ' ' ����
	return (sizeof(rec_t));
}

//	DB ���Ͽ� ���ڵ� r�� �����Ѵ�.
int
fprintf_rec(rec_t *r)
{
	return(fprintf(fp, "%-10d %-11s %-15s %1d %-38s\n", \
		r->key, r->name, r->dep, r->grade, r->addr));
}

//	DB ���Ͽ��� ������ ��������� �о� �ش� ������ �����Ѵ�
int
fscanf_hd(head_t *h) {
	// scanf()ó�� �Է��� �޵� Ű���尡 �ƴ�
	// ����fp���� �Է� ����
	return(fscanf(fp, "%s %d %d %s %f %d %d %d\n",
		h->name, &h->head_sz, &h->rec_sz, h->program, \
		&h->version, &h->start_key, &h->rec_num, &h->fio_type));
	// %s�� ����ü ��� �迭�̸�, �� �� %d %f�� ����� �ּ�
}

//	DB ���Ͽ� ������ ������� ���� ���� �����Ѵ�.
int
fprintf_hd(head_t *h) {
	// printf() ó�� ����ϵ� ȭ���� �ƴ϶� ���� fp�� ���
	return(fprintf(fp, "%-19s %-2d %-2d %-19s %-3.1f %-6d %-6d %1d\n", \
		h->name, h->head_sz, h->rec_sz, h->program, \
		h->version, h->start_key, h->rec_num, h->fio_type));
}

//===========================================================================
// TEXT FILE I/O ���� �Լ��� : C ǥ�� sscanf() sprintf() fgets() fputs()
//===========================================================================

static char sbuf[BUF_LEN];

//	DB ���Ͽ��� ���ڵ� �ϳ��� �о� �޸� r�� �����Ѵ�.
int
sscanf_rec(rec_t *r) {
	// fp ���Ͽ��� �� ��(��), �� ��\n������ �о� �鿩 �޸� sbuf[]�� ����
	if (fgets(sbuf, BUF_LEN, fp) == NULL)
		return(0); // ���� �߻�
				   // fscanf_rec() �Լ��� ����
	sscanf(sbuf, "%d %s %s %d", &r->key, r->name, r->dep, &r->grade);
	strcpy(r->addr, sbuf + 41); // �ּҴ� �ܾ ���� ���� ������ ������; %s�Ұ���
	trunc_space(r->addr);
	return(sizeof(rec_t));
}

//	DB ���Ͽ� ���ڵ� r�� �����Ѵ�.
int
sprintf_rec(rec_t *r) {
	// fprintf_rec() �Լ��� ����
	// fprintf()ó�� ����ϵ�, ������ �ƴ϶� �޸� sbuf[]�� ���
	sprintf(sbuf, "%-10d %-11s %-15s %1d %-38s\n", \
		r->key, r->name, r->dep, r->grade, r->addr);
	return(fputs(sbuf, fp)); // sbuf[]�� ���ڿ��� ���� fp�� ��
}

//	DB ���Ͽ��� ������ ��������� �о� �ش� ������ �����Ѵ�.
int
sscanf_hd(head_t *h) {
	// fp ���Ͽ��� �� ��(��), �� ��\n������ �о� �鿩 �޸� sbuf[]�� �����Ѵ�.
	if (fgets(sbuf, BUF_LEN, fp) == NULL)
		return(0); // ���� �߻�
				   // fscanf_hd() �Լ��� fscanf()�� �����Ѵ�.
	return(sscanf(sbuf, "%s %d %d %s %f %d %d %d\n",
		h->name, &h->head_sz, &h->rec_sz, h->program, \
		&h->version, &h->start_key, &h->rec_num, &h->fio_type));
	// fscanf()ó�� �Է��� �޵� ������ �ƴ϶� �޸� sbuf[]���� �Է��� ����
}

//	DB ���Ͽ� ������ ������� ���� ���� �����Ѵ�.
int
sprintf_hd(head_t *h) {
	// fprintf_hd() �Լ��� fprintf()�� ����
	// fprintf()ó�� ����ϵ�, ������ �ƴ϶� �޸� sbuf[]�� ���
	sprintf(sbuf, "%-19s %-2d %-2d %-19s %-3.1f %-6d %-6d %1d\n", \
		h->name, h->head_sz, h->rec_sz, h->program, \
		h->version, h->start_key, h->rec_num, h->fio_type);
	return(fputs(sbuf, fp)); // sbuf[]�� ���ڿ��� ���� fp�� ��
}

//===========================================================================
// Binary FILE I/O ���� �Լ��� : C ǥ�� fread() fwrite()
//===========================================================================

//	DB ���Ͽ��� ���ڵ� �ϳ��� �о� �޸� r�� �����Ѵ�.
int
fread_rec(rec_t *r) {
	//fwrite(���Ͽ��� �о�� ������ ������ �޸� �ּ�, ����������ũ��, �����ǰ���, fp);
	return(fread(&r, sizeof(rec_t), 1, fp));
}

//	DB ���Ͽ� ���ڵ� r�� �����Ѵ�.
int
fwrite_rec(rec_t *r) {
	//fwrite(���Ͽ� �� �޸� �ּ�, ����������ũ��, �����ǰ���, fp);
	return(fwrite(&r, sizeof(rec_t), 1, fp));
}

//	DB ���Ͽ��� ������ ��������� �о� �ش� ������ �����Ѵ�.
int
fread_hd(head_t *h) {
	// fwrite()�� ������� ������� �о� ���� ��
	//fread(���Ͽ��� �о�� ������ ������ �޸� �ּ�, ����������ũ��, �����ǰ���, fp);
	fread(h->name, sizeof(char), NAME_LEN, fp);
	fread(&h->head_sz, sizeof(int), 1, fp);
	fread(&h->rec_sz, sizeof(int), 1, fp);
	fread(h->program, sizeof(char), NAME_LEN, fp);
	fread(&h->version, sizeof(float), 1, fp);
	fread(&h->start_key, sizeof(int), 1, fp);
	fread(&h->rec_num, sizeof(int), 1, fp);
	fread(&h->fio_type, sizeof(int), 1, fp);
	// ������ ����鵵 �̷� ������ �ۼ��� ��
	return(sizeof(head_t)); // ������ �߻����� �ʾҴٰ� �����ϰ� ��� ũ�� ����

}

//	DB ���Ͽ� ������ ������� ���� ���� �����Ѵ�.
int
fwrite_hd(head_t *h) {
	// char �迭�� �̸��� �ּ���, int�� float ����� �ּҸ� ���� ������ �־�� ��
	//fwrite(���Ͽ� �� �޸� �ּ�, ����������ũ��, �����ǰ���, fp);
	fwrite(h->name, sizeof(char), NAME_LEN, fp);
	fwrite(&h->head_sz, sizeof(int), 1, fp);
	fwrite(&h->rec_sz, sizeof(int), 1, fp);
	fwrite(h->program, sizeof(char), NAME_LEN, fp);
	fwrite(&h->version, sizeof(float), 1, fp);
	fwrite(&h->start_key, sizeof(int), 1, fp);
	fwrite(&h->rec_num, sizeof(int), 1, fp);
	fwrite(&h->fio_type, sizeof(int), 1, fp);
	// ������ ����鵵 �̷� ������ �ۼ��� ��
	return(sizeof(head_t)); // ������ �߻����� �ʾҴٰ� �����ϰ� ��� ũ�� ����
}


//===========================================================================
// Binary FILE I/O ���� �Լ��� : UNIX API open() closde() read() write()
//===========================================================================

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static int fd;

int
r_open_fd(char *name) {	// ���� �ҷ����� : �б������ ����
	return((fd = open(name, O_RDONLY)));
}

int
w_open_fd(char *name) { // �����ϱ�: ������ ��������� ����
	return((fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0644)));
}

int
n_open_fd(char *name) { // �ٸ�(��) �̸����� �����ϱ�:
						// �����, ������ ������ ����, ������ ���� ����
	return((fd = open(name, O_WRONLY | O_CREAT | O_EXCL, 0644)));
}

int
rw_open_fd(char *name) { // ������ �б�/��������� ���� (���� ������ ����)
						 // ���� ���ڵ� ���� ���� �޴� ���� �� ȣ���
	return((fd = open(name, O_RDWR)));
}

int
rwc_open_fd(char *name) { // ������ �б�/��������� ���� (���� ������ ����)
	return((fd = open(name, O_RDWR | O_CREAT, 0644)));
}

//	DB ������ �ݴ´�.
int
close_fd(void) { // fd ���� �ܱ�
	return(close(fd));
}

//	DB ���Ͽ��� ���ڵ� �ϳ��� �о� �޸� r�� �����Ѵ�.
int
read_rec(rec_t *r) {
	//read(���ϱ����, ���Ͽ��� �о� �� ������ ������ �޸� �ּ�, �޸� ũ��);
	return(read(fd, r, sizeof(rec_t)));
}

//	DB ���Ͽ� ���ڵ� r�� �����Ѵ�.
int
write_rec(rec_t *r) {
	//write(���ϱ����, ���Ͽ� �� �޸� �ּ�, �޸� ũ��(���� �� ������ ����));
	return(write(fd, r, sizeof(rec_t)));
}

//	DB ���Ͽ��� ������ ��������� �о� �ش� ������ �����Ѵ�.
int
read_hd(head_t *h) { // write()�� ������� ������� �о� ���� ��
	//read(fd, ���Ͽ��� �о� �� ������ ������ �޸� �ּ�, �޸� ũ��);
	read(fd, h->name, NAME_LEN);
	read(fd, &h->head_sz, sizeof(int));
	read(fd, &h->rec_sz, sizeof(int));
	read(fd, h->program, NAME_LEN);
	read(fd, &h->version, sizeof(float));
	read(fd, &h->start_key, sizeof(int));
	read(fd, &h->rec_num, sizeof(int));
	read(fd, &h->fio_type, sizeof(int));

	return(sizeof(head_t)); // ������ �߻����� �ʾҴٰ� �����ϰ� ��� ũ�� ����
}

//	DB ���Ͽ� ������ ������� ���� ���� �����Ѵ�.
int
write_hd(head_t *h) { // ���Ͽ� ��� h�� �����Ѵ�
	// char �迭�� �̸��� �ּ���, int�� float ����� �ּҸ� ���� ������ �־�� ��
	//write(fd, ���Ͽ� �� �޸� �ּ�, �޸� ũ��(���� �� ������ ����));
	write(fd, h->name, NAME_LEN);
	write(fd, &h->head_sz, sizeof(int));
	write(fd, &h->rec_sz, sizeof(int));
	write(fd, h->program, NAME_LEN);
	write(fd, &h->version, sizeof(float));
	write(fd, &h->start_key, sizeof(int));
	write(fd, &h->rec_num, sizeof(int));
	write(fd, &h->fio_type, sizeof(int));

	return(sizeof(head_t));
}

int 
setpos_fd(int pos) { // fd ������ �а� ���� ��ġ(file position)�� pos�� �ű�
	return(lseek(fd, SEEK_SET, pos)); // SEEK_SET, SEEK_END, SEEK_CUR
}

#endif  /* _FILE_IO_C */

