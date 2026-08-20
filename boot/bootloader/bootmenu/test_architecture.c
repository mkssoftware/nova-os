#include "test_architecture.h"

static nova_boot_test_case_t cases[NOVA_BOOT_TEST_CAPACITY];
static nova_test_result_t results[NOVA_BOOT_TEST_CAPACITY];
static nova_boot_test_summary_t summary;
static nova_boot_test_clock_fn test_clock;
static nova_boot_test_state_fn state_probe;

bool nova_boot_test_initialize(nova_boot_test_clock_fn clock,
                               nova_boot_test_state_fn probe)
{
    for(uint32_t i=0;i<NOVA_BOOT_TEST_CAPACITY;++i){cases[i]=(nova_boot_test_case_t){0};
        results[i]=(nova_test_result_t){0};}
    summary=(nova_boot_test_summary_t){.initialized=true,.deterministic=true,
        .isolated=true,.boot_data_unchanged=true,.security_preserved=true};
    test_clock=clock;state_probe=probe;return true;
}

bool nova_boot_test_register(const nova_boot_test_case_t *test_case)
{
    if(!summary.initialized||!test_case||!test_case->test_id||!test_case->suite_id||
       !test_case->data_version||!test_case->name||!test_case->description||
       !test_case->execute||test_case->level>NOVA_TEST_STRESS||
       test_case->kind>NOVA_TEST_ROBUSTNESS||summary.registered>=NOVA_BOOT_TEST_CAPACITY)
        return false;
    for(uint32_t i=0;i<summary.registered;++i)
        if(cases[i].test_id==test_case->test_id)return false;
    cases[summary.registered++]=*test_case;return true;
}

bool nova_boot_test_run(uint32_t suite)
{
    if(!summary.initialized)return false;
    summary.executed=summary.passed=summary.failed=summary.skipped=0;
    summary.total_duration_us=summary.average_duration_us=0;summary.isolated=true;
    for(uint32_t i=0;i<summary.registered;++i){
        if(suite!=NOVA_BOOT_TEST_ALL_SUITES&&cases[i].suite_id!=suite)continue;
        nova_test_result_t *result=&results[summary.executed++];
        *result=(nova_test_result_t){.test_id=cases[i].test_id,.suite_id=cases[i].suite_id,
            .expected=cases[i].expected,.module=cases[i].module,.status=NOVA_TEST_RUNNING,
            .isolated=true};
        result->state_before=state_probe?state_probe():0;
        uint64_t start=test_clock?test_clock():0;
        result->status=cases[i].execute(cases[i].context,&result->actual,&result->error_code);
        uint64_t end=test_clock?test_clock():start;
        result->duration_us=end>=start?end-start:0;
        result->state_after=state_probe?state_probe():result->state_before;
        result->isolated=result->state_before==result->state_after;
        if(!result->isolated){result->status=NOVA_TEST_FAILED;result->error_code=0x1001u;
            summary.isolated=false;}
        if(result->status==NOVA_TEST_PASSED&&result->actual!=result->expected){
            result->status=NOVA_TEST_FAILED;result->error_code=0x1002u;}
        if(result->status==NOVA_TEST_PASSED)++summary.passed;
        else if(result->status==NOVA_TEST_SKIPPED)++summary.skipped;
        else ++summary.failed;
        summary.total_duration_us+=result->duration_us;
    }
    ++summary.repetitions;
    if(summary.executed)summary.average_duration_us=summary.total_duration_us/summary.executed;
    return summary.executed&&summary.failed==0;
}

const nova_test_result_t *nova_boot_test_results(void){return results;}
const nova_boot_test_summary_t *nova_boot_test_summary(void){return &summary;}
static bool append(uint8_t *o,uint32_t c,uint32_t *p,const char *s)
{while(*s){if(*p+1>=c)return false;o[(*p)++]=(uint8_t)*s++;}o[*p]=0;return true;}
static bool number(uint8_t *o,uint32_t c,uint32_t *p,uint64_t v)
{char d[20];uint8_t n=0;do{d[n++]=(char)('0'+v%10u);v/=10u;}while(v&&n<20);
 while(n){if(*p+1>=c)return false;o[(*p)++]=(uint8_t)d[--n];}o[*p]=0;return true;}
bool nova_boot_test_generate_report(bool authorized,uint8_t *output,
                                    uint32_t capacity,uint32_t *written)
{
    if(written)*written=0;
    if(!authorized||!output||!written||capacity<192||!summary.initialized)return false;
    uint32_t p=0;
    if(!append(output,capacity,&p,"NOVA_BOOT_TEST_REPORT\nregistered=")||
       !number(output,capacity,&p,summary.registered)||!append(output,capacity,&p," executed=")||
       !number(output,capacity,&p,summary.executed)||!append(output,capacity,&p," passed=")||
       !number(output,capacity,&p,summary.passed)||!append(output,capacity,&p," failed=")||
       !number(output,capacity,&p,summary.failed)||!append(output,capacity,&p," skipped=")||
       !number(output,capacity,&p,summary.skipped)||!append(output,capacity,&p," total_us=")||
       !number(output,capacity,&p,summary.total_duration_us)||!append(output,capacity,&p," average_us=")||
       !number(output,capacity,&p,summary.average_duration_us)||!append(output,capacity,&p,"\n"))return false;
    for(uint32_t i=0;i<summary.executed;++i)if(!append(output,capacity,&p,"test=")||
       !number(output,capacity,&p,results[i].test_id)||!append(output,capacity,&p," suite=")||
       !number(output,capacity,&p,results[i].suite_id)||!append(output,capacity,&p," status=")||
       !number(output,capacity,&p,results[i].status)||!append(output,capacity,&p," duration_us=")||
       !number(output,capacity,&p,results[i].duration_us)||!append(output,capacity,&p," error=")||
       !number(output,capacity,&p,results[i].error_code)||!append(output,capacity,&p,"\n"))return false;
    *written=p;return true;
}
