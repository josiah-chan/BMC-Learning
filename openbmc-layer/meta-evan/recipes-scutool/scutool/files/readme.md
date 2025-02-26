CREATE TABLE scu_registers (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    register_name CHAR(6) NOT NULL,    -- 修正为 6 个字符，符合 SCU000 到 SCUFFC
    description VARCHAR(100) NOT NULL, -- 描述内容，最大长度 100 个字符
    offset CHAR(4) NOT NULL,           -- 偏移地址，格式如 000h 到 FFCh
    base_address TEXT DEFAULT '0x1E6E2000' NOT NULL, -- 统一默认值
    full_address TEXT GENERATED ALWAYS AS ('1E6E2' || SUBSTR(offset, 1, LENGTH(offset)-1)) STORED, -- 自动计算完整地址
    UNIQUE (register_name, offset)     -- 确保每个寄存器名称和偏移地址组合唯一
);

-- 创建索引以加速查询
CREATE INDEX idx_scu_registers_offset ON scu_registers(offset);
CREATE INDEX idx_scu_registers_register_name ON scu_registers(register_name);

-- 插入第一条数据：SCU000 - Protection Key Register
INSERT INTO scu_registers (register_name, description, offset)
VALUES ('SCU000', 'Protection Key Register', '000h');

-- 插入 SCU010: Protection Key Register 2
INSERT INTO scu_registers (register_name, description, offset)
VALUES ('SCU010', 'Protection Key Register 2', '010h');

-- 插入 SCU004: Silicon Revision ID Register
INSERT INTO scu_registers (register_name, description, offset)
VALUES ('SCU004', 'Silicon Revision ID Register', '004h');

-- 插入 SCU014: Silicon Revision ID Register 2
INSERT INTO scu_registers (register_name, description, offset)
VALUES ('SCU014', 'Silicon Revision ID Register 2', '014h');

-- 插入最后一条数据：SCUFFC - Reset Source Control Register #17
INSERT INTO scu_registers (register_name, description, offset)
VALUES ('SCUFFC', 'Reset Source Control Register #17', 'FFCh');

g++ -o scutool main.cpp Sqlite3Database.cpp -lsqlite3