
def read_TSV(tsv_file, read_from_string=False):
    if read_from_string:
        tsv_data_lines = tsv_file.rstrip('\n').split('\n')
        tsv_data_list = [line.split('\t') for line in tsv_data_lines]
        return tsv_data_list
    else:
        with open(tsv_file) as data_reader:
            tsv_data_list = [line.rstrip('\n').split('\t') for line in data_reader]
        return tsv_data_list


def write_TSV(result, out_file="result.txt", write_to_string=False):
    if write_to_string:
        result_tsv = '\n'.join(['\t'.join(line) for line in result])
        return result_tsv
    else:
        with open(out_file, 'w') as data_writer:
            for line in result:
                print >> data_writer, '\t'.join(map(str, line))


def get_parameter(config_file, key_num):
    with open(config_file) as data_reader:
        pm = data_reader.readlines()[key_num].rstrip('\n')
    return pm


def retain_wanted(origin_data, wanted_list):
    type_of_data = type(origin_data)
    if type_of_data == dict:
        return {key: origin_data[key] for key in wanted_list if key in origin_data}
    elif type_of_data == list:
        return [value for value in wanted_list if value in origin_data]
