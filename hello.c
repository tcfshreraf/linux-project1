#include <linux/kernel.h>
#include<linux/kernel.h>
#include<linux/syscalls.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/proc_fs.h>
#include<linux/pid.h>
#include<linux/sched.h>
#include<linux/mm.h>
#include<linux/mm_types.h>
#include<linux/highmem.h>
#include<linux/string.h>

static unsigned long convert_to_phy_add(struct mm_struct *mm, unsigned long vir_add)
{
	unsigned long phy_add;
	pgd_t *pgd = NULL;
	pud_t *pud = NULL;
	pmd_t *pmd = NULL;
	pte_t *pte = NULL;
	p4d_t *p4d = NULL;
	struct page *pg = NULL;

	pgd = pgd_offset(mm, vir_add);
	if(pgd_none(*pgd) || pgd_bad(*pgd))
		return 0;

	p4d = p4d_offset(pgd, vir_add);
	if(p4d_none(*p4d) || p4d_bad(*p4d))
		return 0;

	pud = pud_offset(p4d, vir_add);
	if(pud_none(*pud) || pud_bad(*pud))
		return 0;

	pmd = pmd_offset(pud, vir_add);
	if(pmd_none(*pmd) || pmd_bad(*pmd))
		return 0;
	
	
	if (!(pte = pte_offset_map(pmd, vir_add)))
                return 0;

        if (!(pg = pte_page(*pte)))
                return 0;

        phy_add = page_to_phys(pg);
        pte_unmap(pte);
        return phy_add;

}
void vm_table(struct task_struct *task,char* result)
{
        struct vm_area_struct *cur = task->mm->mmap;

	//int count_null = 0, count_all = 0;
        while(cur != NULL)
        {
                unsigned long start = cur->vm_start;
                unsigned long end = cur->vm_end;
		printk("%lu %lu\n", start, convert_to_phy_add(task->mm, start));
		printk("%lu %lu\n", end, convert_to_phy_add(task->mm, end));
		cur = cur -> vm_next;
	}

}

asmlinkage long sys_hello(char* result)
{
        struct pid *pid_struct;
        struct task_struct *task = get_current();
        //printk("%d", task->pid);
        pid_struct = find_get_pid(task->pid);



        vm_table(task,result);
        return 0;
}