--------------------------------------------------------
--  Config part
---------------------------------------------------------
local root_path = "/srv/www/dashengine.com/public_html/"
local store_path = "download/"
local public_ip = "www.dashengine.com"
---------------------------------------------------------
--  Config part
---------------------------------------------------------

local upload = require "resty.upload"
local cjson = require "cjson"
local chunk_size = 8192

local file_handle
local file_len = 0 
local file_name
local str_today_and_md5

function get_filename(res)  
        --ngx.log(ngx.ERR, "*****RES=", res);
        local filename = ngx.re.match(res,'(.+)name="-(.+)-"(.*)')  
        if filename then   
                return filename[2]  
        end  
end  

function is_md5(str)
        --ngx.log(ngx.ERR, "ERR|file name=", str, ",len=", string.len(str))
        if(string.len(str) ~= 32) then 
                return 
        end

        local i=1
        while i<=32 do
                local _c = string.byte(str, i)
                if ( not(_c>=48 and _c<=57 or _c>=97 and _c<=102) ) then                                -- '0'=48 '9'=57 'a'=97 'f'=102
                        return
                end
                i=i+1
        end
        return true
end


local form, err = upload:new(chunk_size)
if not form then
    ngx.log(ngx.ERR, "ERR|failed to new upload: ", err)
    ngx.exit(500)
end

form:set_timeout(1000*10) -- 10 sec

while true do
        local typ, res, err = form:read()
        if not typ then
                ngx.say("ERR|failed to read: ", err)
                        return
        end
    
        if typ == "header" then  
                if res[1] == "Content-Type" then
			ngx.log(ngx.ERR, "ERR|Content-Type=", res[2]);
		else  
                        file_name = get_filename(res[2])  
                        if file_name  and is_md5(file_name) then  
                                str_today_and_md5 = ngx.today() .. "/" ..  string.sub(file_name, 1, 2) .. "/" .. string.sub(file_name, 3, 4)
                                local _filepath = root_path .. store_path  .. str_today_and_md5
                                local _filename = _filepath .. "/" .. file_name  
                                file_handle = io.open(_filename, "w+") 
                                         
                                if not file_handle then  
                                        -- create path
                                        os.execute("mkdir -p " .. _filepath)
                                        file_handle = io.open(_filename, "w+")  
                                end

                                if not file_handle then  
                                        ngx.say("ERR|failed to open file ")  
                                        return  
                                end 
                        end  
                end      
        elseif typ == "body" then
                if file_handle then  
                        file_len= file_len + tonumber(string.len(res))      
                        file_handle:write(res)  
                end  
   elseif typ == "part_end" then
                if file_handle then  
                        file_handle:close()  
                        file_handle = nil  

                        --return doanload path
                        local download_url = "http://" .. public_ip .. "/" .. store_path .. str_today_and_md5 .. "/" .. file_name
                        ngx.say("SUCCESS|", download_url)  
                end      
        elseif typ == "eof" then
                break
        end
end

