
name sx::parse_memo_referrer( const vector<string> tokens )
{
    if ( tokens.size() == 0 || tokens.size() == 1 ) return ""_n;
    return name{ tokens[1] };
}