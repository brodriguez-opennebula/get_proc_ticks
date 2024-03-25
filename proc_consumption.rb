#!/usr/bin/ruby

require 'json'
require 'base64'
require 'uri'
require 'net/http'

def help_message()
  puts "Usage: #{ARGV[-1]} vm_id[[,vm_id]*]"
  exit
end

def query_vm_agent(vm_id, query, host='qemu:///system')
  cmd = "virsh -c #{host} qemu-agent-command one-#{vm_id} '#{query.to_json}'"
  out = %x[ #{cmd} ]
  return JSON.parse(out)
end 

def get_vm_os(vm_id)
  query = {'execute': 'guest-get-osinfo'}
  res = query_vm_agent(vm_id, query)
end 

def get_scaphandre_data(host = "localhost", port="8080")
  uri = URI("http://#{host}:#{port}/metrics")
  res = Net::HTTP.get_response(uri)
  if res.is_a?(Net::HTTPOK) then
    return res.body 
  else 
    return "" 
  end
end

def grep_power_from_scaphandre_data(vm_id, res)
  power = (res.split("\n")).grep(/one-#{vm_id},/)[0]
  power = power.split().last.to_i
  return power
end

def issue_command(vm_id, cmd, args)
  query_processes={ 'execute': 'guest-exec', 'arguments': { 'path': cmd, 'arg': args.split(), 'capture-output': true }}
  out = query_vm_agent(vm_id, query_processes)

  pid=out['return']['pid']
  return pid
end

def retrieve_stdout_command(vm_id, pid)
  query_get_stdout={ 'execute': 'guest-exec-status', 'arguments': { 'pid': pid } }
  out = query_vm_agent(vm_id, query_get_stdout)

  begin
    procs_info=Base64.decode64(out['return']['out-data'])
    return procs_info
  rescue
    return ""
  end
end

def get_proc_consumption(vm_id, full_power=0, interval=5)
    pid_cmd = issue_command(vm_id, '/opt/opennebula/proc_consumption',"-s #{interval} -m #{vm_id} -p #{full_power}")
    sleep interval+0.1
    procs_info = retrieve_stdout_command(vm_id, pid_cmd)
    puts procs_info unless procs_info==""
end

help_message if ARGV.length==0

res = get_scaphandre_data()

power = {}
threads = []

ARGV[0].split(',').each() { |vm_id|
    power ["vm_id"] = grep_power_from_scaphandre_data(vm_id, res)
    threads << Thread.new { get_proc_consumption(vm_id, power["vm_id"]) }
}

threads.each(&:join)
